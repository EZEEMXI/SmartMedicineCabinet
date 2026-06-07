#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QObject>
#include <QSqlDatabase>
#include <QVariantList>
#include <QVariantMap>

/**
 * 本地 SQLite 数据库管理
 *
 * 表结构：
 *   cart    (id INTEGER PRIMARY KEY AUTOINCREMENT, name TEXT, price REAL, num INTEGER)
 *   orders  (id INTEGER PRIMARY KEY AUTOINCREMENT, time TEXT, price REAL, content TEXT)
 */
class DatabaseManager : public QObject
{
    Q_OBJECT

public:
    explicit DatabaseManager(QObject *parent = nullptr);
    ~DatabaseManager();

    Q_INVOKABLE bool initialize();

    // ── 购物车 ────────────────────────────────
    Q_INVOKABLE bool addToCart(const QString &name, double price);
    Q_INVOKABLE bool updateCartNum(int id, int delta);  // +1 / -1
    Q_INVOKABLE bool removeFromCart(int id);
    Q_INVOKABLE QVariantList getAllCart();
    Q_INVOKABLE int getCartCount();

    // ── 订单 ──────────────────────────────────
    Q_INVOKABLE bool submitOrder();                     // 购物车 → 订单，清空购物车
    Q_INVOKABLE QVariantList getAllOrders();
    Q_INVOKABLE bool deleteOrder(int id);

private:
    QSqlDatabase m_db;
    QString m_dbPath;
};

#endif // DATABASEMANAGER_H
