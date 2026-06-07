#include "databasemanager.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDateTime>
#include <QStandardPaths>
#include <QDir>
#include <QDebug>

DatabaseManager::DatabaseManager(QObject *parent)
    : QObject(parent)
{
}

DatabaseManager::~DatabaseManager()
{
    if (m_db.isOpen()) m_db.close();
}

bool DatabaseManager::initialize()
{
    // 使用应用数据目录
    QString dataDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(dataDir);
    m_dbPath = dataDir + "/smartcabinet.db";

    m_db = QSqlDatabase::addDatabase("QSQLITE");
    m_db.setDatabaseName(m_dbPath);

    if (!m_db.open()) {
        qWarning() << "无法打开数据库:" << m_db.lastError().text();
        return false;
    }

    QSqlQuery q(m_db);

    // 购物车表
    q.exec("CREATE TABLE IF NOT EXISTS cart ("
           "id INTEGER PRIMARY KEY AUTOINCREMENT,"
           "name TEXT NOT NULL,"
           "price REAL NOT NULL,"
           "num INTEGER NOT NULL DEFAULT 1)");

    // 订单表
    q.exec("CREATE TABLE IF NOT EXISTS orders ("
           "id INTEGER PRIMARY KEY AUTOINCREMENT,"
           "time TEXT NOT NULL,"
           "price REAL NOT NULL,"
           "content TEXT NOT NULL)");

    qDebug() << "数据库已初始化:" << m_dbPath;
    return true;
}

// ────────────────────────────────────────────────────────────
//  购物车
// ────────────────────────────────────────────────────────────

bool DatabaseManager::addToCart(const QString &name, double price)
{
    QSqlQuery q(m_db);
    // 检查是否已存在
    q.prepare("SELECT id, num FROM cart WHERE name = ?");
    q.addBindValue(name);
    if (q.exec() && q.next()) {
        int id = q.value(0).toInt();
        int num = q.value(1).toInt();
        q.prepare("UPDATE cart SET num = ?, price = ? WHERE id = ?");
        q.addBindValue(num + 1);
        q.addBindValue(price * (num + 1));
        q.addBindValue(id);
        return q.exec();
    } else {
        q.prepare("INSERT INTO cart (name, price, num) VALUES (?, ?, 1)");
        q.addBindValue(name);
        q.addBindValue(price);
        return q.exec();
    }
}

bool DatabaseManager::updateCartNum(int id, int delta)
{
    QSqlQuery q(m_db);
    q.prepare("SELECT name, num, price FROM cart WHERE id = ?");
    q.addBindValue(id);
    if (!q.exec() || !q.next()) return false;

    QString name = q.value(0).toString();
    int num = q.value(1).toInt();
    double totalPrice = q.value(2).toDouble();
    double unitPrice = totalPrice / num;

    int newNum = num + delta;
    if (newNum <= 0) {
        return removeFromCart(id);
    }

    q.prepare("UPDATE cart SET num = ?, price = ? WHERE id = ?");
    q.addBindValue(newNum);
    q.addBindValue(unitPrice * newNum);
    q.addBindValue(id);
    return q.exec();
}

bool DatabaseManager::removeFromCart(int id)
{
    QSqlQuery q(m_db);
    q.prepare("DELETE FROM cart WHERE id = ?");
    q.addBindValue(id);
    return q.exec();
}

QVariantList DatabaseManager::getAllCart()
{
    QVariantList list;
    QSqlQuery q("SELECT id, name, price, num FROM cart ORDER BY id", m_db);
    while (q.next()) {
        QVariantMap item;
        item["id"]    = q.value(0).toInt();
        item["name"]  = q.value(1).toString();
        item["price"] = q.value(2).toDouble();
        item["num"]   = q.value(3).toInt();
        list.append(item);
    }
    return list;
}

int DatabaseManager::getCartCount()
{
    QSqlQuery q("SELECT COALESCE(SUM(num), 0) FROM cart", m_db);
    if (q.next()) return q.value(0).toInt();
    return 0;
}

// ────────────────────────────────────────────────────────────
//  订单
// ────────────────────────────────────────────────────────────

bool DatabaseManager::submitOrder()
{
    QSqlQuery q(m_db);

    // 收集购物车内容
    QVariantList cart = getAllCart();
    if (cart.isEmpty()) return false;

    QStringList items;
    double totalPrice = 0.0;
    for (const QVariant &v : cart) {
        QVariantMap item = v.toMap();
        QString name = item["name"].toString();
        int num = item["num"].toInt();
        double price = item["price"].toDouble();
        items.append(QString("%1 x%2").arg(name).arg(num));
        totalPrice += price;
    }

    QString time = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    QString content = items.join("; ");

    // 写入订单
    q.prepare("INSERT INTO orders (time, price, content) VALUES (?, ?, ?)");
    q.addBindValue(time);
    q.addBindValue(totalPrice);
    q.addBindValue(content);
    if (!q.exec()) return false;

    // 清空购物车
    q.exec("DELETE FROM cart");
    return true;
}

QVariantList DatabaseManager::getAllOrders()
{
    QVariantList list;
    QSqlQuery q("SELECT id, time, price, content FROM orders ORDER BY id DESC", m_db);
    while (q.next()) {
        QVariantMap item;
        item["id"]      = q.value(0).toInt();
        item["time"]    = q.value(1).toString();
        item["price"]   = q.value(2).toDouble();
        item["content"] = q.value(3).toString();
        list.append(item);
    }
    return list;
}

bool DatabaseManager::deleteOrder(int id)
{
    QSqlQuery q(m_db);
    q.prepare("DELETE FROM orders WHERE id = ?");
    q.addBindValue(id);
    return q.exec();
}
