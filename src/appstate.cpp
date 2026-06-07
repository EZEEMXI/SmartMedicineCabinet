#include "appstate.h"
#include "mqttclient.h"
#include "databasemanager.h"
#include <QSettings>
#include <QDebug>
#include <QCryptographicHash>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>

AppState* AppState::s_instance = nullptr;

AppState::AppState(QObject *parent)
    : QObject(parent)
    , m_mqtt(new MqttClient(this))
    , m_db(new DatabaseManager(this))
{
    s_instance = this;
}

AppState* AppState::instance()
{
    return s_instance;
}

void AppState::init()
{
    m_db->initialize();

    connect(m_mqtt, &MqttClient::connectedChanged, this, [this]() {
        m_mqttConnected = m_mqtt->connected();
        m_mqttStatus = m_mqtt->statusText();
        emit mqttConnectedChanged();
        emit mqttStatusChanged();
    });

    connect(m_mqtt, &MqttClient::statusTextChanged, this, [this]() {
        m_mqttStatus = m_mqtt->statusText();
        emit mqttStatusChanged();
    });

    connect(m_mqtt, &MqttClient::propertyReceived, this, &AppState::onPropertyReceived);
    connect(m_mqtt, &MqttClient::logMessage, this, &AppState::logMessage);

    // ── OneNet 凭证（从 QSettings 读取，首次使用前须在 App 设置页填写）──
    // 不在代码里硬编码密钥；真实值保存在本机 secrets.json
    const QString defaultPid = "";
    const QString defaultDn  = "";
    const QString defaultDk  = "";
    const QString defaultAk  = "";

    // QSettings 可覆盖默认值
    QString pid = getSetting("productId", defaultPid);
    QString dn  = getSetting("deviceName", defaultDn);
    QString dk  = getSetting("deviceKey", defaultDk);
    QString ak  = getSetting("accessKey", defaultAk);

    setOneNetConfig(pid, dn, dk, ak);
    connectMqtt();
}

// ────────────────────────────────────────────────────────────
//  MQTT
// ────────────────────────────────────────────────────────────

void AppState::setOneNetConfig(const QString &productId,
                               const QString &deviceName,
                               const QString &deviceKey,
                               const QString &accessKey)
{
    // 空值回退：不内置默认密钥，必须由用户在 App 设置页配置
    const QString pid = productId.isEmpty()  ? "" : productId;
    const QString dn  = deviceName.isEmpty() ? "" : deviceName;
    const QString dk  = deviceKey.isEmpty()  ? "" : deviceKey;
    const QString ak  = accessKey.isEmpty()  ? "" : accessKey;
    m_mqtt->setOneNetConfig(pid, dn, dk, ak);
}

void AppState::connectMqtt()
{
    m_mqtt->connectToHost();
}

void AppState::disconnectMqtt()
{
    m_mqtt->disconnectFromHost();
}

// ────────────────────────────────────────────────────────────
//  用户登录 / 注册 / 信息
// ────────────────────────────────────────────────────────────

static QByteArray hashPassword(const QString &pw)
{
    return QCryptographicHash::hash(pw.toUtf8(), QCryptographicHash::Sha256).toHex();
}

static QJsonArray loadUsers()
{
    QSettings s("SmartCabinet", "SmartCabinet");
    QByteArray data = s.value("users").toByteArray();
    if (data.isEmpty()) return QJsonArray();
    return QJsonDocument::fromJson(data).array();
}

static void saveUsers(const QJsonArray &arr)
{
    QSettings s("SmartCabinet", "SmartCabinet");
    s.setValue("users", QJsonDocument(arr).toJson(QJsonDocument::Compact));
}

// 首次运行时植入管理员账号
static void seedAdminIfEmpty()
{
    QJsonArray users = loadUsers();
    if (!users.isEmpty()) return;
    QJsonObject admin;
    admin["n"] = "XR";
    admin["h"] = QString::fromLatin1(hashPassword("123456"));
    admin["a"] = true;
    users.append(admin);
    saveUsers(users);
}

bool AppState::login(const QString &username, const QString &password)
{
    // 空值检查
    if (username.trimmed().isEmpty()) {
        qWarning() << "[App] 登录失败: 用户名为空";
        return false;
    }
    if (password.isEmpty()) {
        qWarning() << "[App] 登录失败: 密码为空";
        return false;
    }

    seedAdminIfEmpty();
    QJsonArray users = loadUsers();

    for (int i = 0; i < users.size(); i++) {
        QJsonObject u = users[i].toObject();
        if (u["n"].toString() == username) {
            if (u["h"].toString() != QString::fromLatin1(hashPassword(password)))
                return false;  // 密码错误

            m_isAdmin   = u["a"].toBool(false);
            m_userName  = u["n"].toString();
            m_userPhone = u["p"].toString("");
            m_userAddress = u["d"].toString("");
            emit isAdminChanged();
            emit userNameChanged();
            emit userInfoChanged();
            return true;
        }
    }
    return false;  // 用户不存在
}

QString AppState::registerUser(const QString &username, const QString &password)
{
    if (username.trimmed().isEmpty()) return "用户名不能为空";
    if (password.length() < 4)         return "密码至少4位";

    QJsonArray users = loadUsers();
    for (int i = 0; i < users.size(); i++) {
        if (users[i].toObject()["n"].toString() == username)
            return "用户名已存在";
    }

    QJsonObject u;
    u["n"] = username.trimmed();
    u["h"] = QString::fromLatin1(hashPassword(password));
    u["a"] = false;  // 新用户默认非管理员
    u["p"] = "";
    u["d"] = "";
    users.append(u);
    saveUsers(users);

    // 注册后自动登录
    m_isAdmin   = false;
    m_userName  = username.trimmed();
    m_userPhone = "";
    m_userAddress = "";
    emit isAdminChanged();
    emit userNameChanged();
    emit userInfoChanged();
    return "";  // 成功
}

void AppState::saveUserInfo(const QString &phone, const QString &address)
{
    m_userPhone = phone;
    m_userAddress = address;
    emit userInfoChanged();

    // 持久化到用户列表
    QJsonArray users = loadUsers();
    for (int i = 0; i < users.size(); i++) {
        QJsonObject u = users[i].toObject();
        if (u["n"].toString() == m_userName) {
            u["p"] = phone;
            u["d"] = address;
            users[i] = u;
            saveUsers(users);
            break;
        }
    }
}

bool AppState::hasOneNetConfig() const
{
    // 始终返回 true —— 内置了默认凭证，无需用户配置
    return true;
}

// ────────────────────────────────────────────────────────────
//  收到设备属性
// ────────────────────────────────────────────────────────────

void AppState::onPropertyReceived(const QJsonObject &params)
{
    qDebug() << "[App] 收到属性:" << params;

    //   辅助函数：兼容两种格式
    //   格式A（设备上报）: {"value": 75}
    //   格式B（属性设置）: 75 (直接是整数)
    auto extractInt = [](const QJsonValue &v) -> int {
        if (v.isObject()) return v.toObject()["value"].toInt();
        return v.toInt();
    };

    if (params.contains("heart_rate")) {
        int val = extractInt(params["heart_rate"]);
        if (val > 0 && val != m_heartRate) {
            m_heartRate = val;
            emit heartRateChanged();
        }
    }

    if (params.contains("spo2")) {
        int val = extractInt(params["spo2"]);
        if (val > 0 && val != m_spo2) {
            m_spo2 = val;
            emit spo2Changed();
        }
    }

    if (params.contains("door_status") || params.contains("cabinet_status")) {
        int val = params.contains("door_status")
                      ? extractInt(params["door_status"])
                      : extractInt(params["cabinet_status"]);
        // bit0 = 柜1, bit1 = 柜2
        int c1 = (val & 1) ? 1 : 0;
        int c2 = (val & 2) ? 1 : 0;
        if (c1 != m_cabinet1Status) { m_cabinet1Status = c1; emit cabinet1StatusChanged(); }
        if (c2 != m_cabinet2Status) { m_cabinet2Status = c2; emit cabinet2StatusChanged(); }
    }
}

// ────────────────────────────────────────────────────────────
//  设置持久化
// ────────────────────────────────────────────────────────────

QString AppState::getSetting(const QString &key, const QString &defaultValue) const
{
    QSettings s("SmartCabinet", "SmartCabinet");
    return s.value(key, defaultValue).toString();
}

void AppState::saveSetting(const QString &key, const QString &value)
{
    QSettings s("SmartCabinet", "SmartCabinet");
    s.setValue(key, value);
}

// ────────────────────────────────────────────────────────────
//  数据库转发
// ────────────────────────────────────────────────────────────

bool AppState::addToCart(const QString &name, double price)
{
    bool ok = m_db->addToCart(name, price);
    if (ok) emit cartCountChanged();
    return ok;
}

bool AppState::updateCartNum(int id, int delta)
{
    bool ok = m_db->updateCartNum(id, delta);
    if (ok) emit cartCountChanged();
    return ok;
}

bool AppState::removeFromCart(int id)
{
    bool ok = m_db->removeFromCart(id);
    if (ok) emit cartCountChanged();
    return ok;
}

QVariantList AppState::getAllCart()
{
    return m_db->getAllCart();
}

bool AppState::submitOrder()
{
    bool ok = m_db->submitOrder();
    if (ok) emit cartCountChanged();
    return ok;
}

QVariantList AppState::getAllOrders()
{
    return m_db->getAllOrders();
}

bool AppState::deleteOrder(int id)
{
    return m_db->deleteOrder(id);
}

int AppState::cartCount()
{
    return m_db->getCartCount();
}
