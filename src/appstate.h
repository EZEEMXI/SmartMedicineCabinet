#ifndef APPSTATE_H
#define APPSTATE_H

#include <QObject>
#include <QJsonObject>

class MqttClient;
class DatabaseManager;

/**
 * 全局应用状态 — QML 与 C++ 的桥梁
 *
 * 单例，在 main.cpp 创建后注册到 QML 上下文
 */
class AppState : public QObject
{
    Q_OBJECT

    // ── MQTT 状态 ─────────────────────────────
    Q_PROPERTY(bool mqttConnected READ mqttConnected NOTIFY mqttConnectedChanged)
    Q_PROPERTY(QString mqttStatus READ mqttStatus NOTIFY mqttStatusChanged)

    // ── 健康数据 ──────────────────────────────
    Q_PROPERTY(int heartRate READ heartRate NOTIFY heartRateChanged)
    Q_PROPERTY(int spo2 READ spo2 NOTIFY spo2Changed)
    Q_PROPERTY(QString heartRateText READ heartRateText NOTIFY heartRateChanged)
    Q_PROPERTY(QString spo2Text READ spo2Text NOTIFY spo2Changed)

    // ── 药柜状态 ──────────────────────────────
    Q_PROPERTY(int cabinet1Status READ cabinet1Status NOTIFY cabinet1StatusChanged)
    Q_PROPERTY(int cabinet2Status READ cabinet2Status NOTIFY cabinet2StatusChanged)

    // ── 购物车 ────────────────────────────────
    Q_PROPERTY(int cartCount READ cartCount NOTIFY cartCountChanged)

    // ── 用户 ─────────────────────────────────
    Q_PROPERTY(QString userName READ userName WRITE setUserName NOTIFY userNameChanged)
    Q_PROPERTY(bool isAdmin READ isAdmin NOTIFY isAdminChanged)
    Q_PROPERTY(bool hasOneNetConfig READ hasOneNetConfig CONSTANT)
    Q_PROPERTY(QString userPhone READ userPhone NOTIFY userInfoChanged)
    Q_PROPERTY(QString userAddress READ userAddress NOTIFY userInfoChanged)

public:
    explicit AppState(QObject *parent = nullptr);

    static AppState* instance();

    // ── 初始化 ────────────────────────────────
    Q_INVOKABLE void init();

    // ── MQTT 操作 ─────────────────────────────
    Q_INVOKABLE void setOneNetConfig(const QString &productId,
                                     const QString &deviceName,
                                     const QString &deviceKey,
                                     const QString &accessKey);
    Q_INVOKABLE void connectMqtt();
    Q_INVOKABLE void disconnectMqtt();

    // ── 用户登录 ──────────────────────────────
    Q_INVOKABLE bool login(const QString &username, const QString &password);
    Q_INVOKABLE QString registerUser(const QString &username, const QString &password);
    // registerUser 返回: ""=成功, 否则=错误信息

    // ── 设置持久化 ────────────────────────────
    Q_INVOKABLE QString getSetting(const QString &key, const QString &defaultValue = "") const;
    Q_INVOKABLE void saveSetting(const QString &key, const QString &value);

    // ── 数据库操作（转发）─────────────────────
    Q_INVOKABLE bool addToCart(const QString &name, double price);
    Q_INVOKABLE bool updateCartNum(int id, int delta);
    Q_INVOKABLE bool removeFromCart(int id);
    Q_INVOKABLE QVariantList getAllCart();
    Q_INVOKABLE bool submitOrder();
    Q_INVOKABLE QVariantList getAllOrders();
    Q_INVOKABLE bool deleteOrder(int id);

    // ── 属性访问 ──────────────────────────────
    bool mqttConnected() const { return m_mqttConnected; }
    QString mqttStatus() const { return m_mqttStatus; }
    int heartRate() const { return m_heartRate; }
    int spo2() const { return m_spo2; }
    QString heartRateText() const { return m_heartRate > 0 ? QString::number(m_heartRate) : "--"; }
    QString spo2Text() const { return m_spo2 > 0 ? QString::number(m_spo2) + "%" : "--%"; }
    int cabinet1Status() const { return m_cabinet1Status; }
    int cabinet2Status() const { return m_cabinet2Status; }
    int cartCount();
    QString userName() const { return m_userName; }
    void setUserName(const QString &name) { m_userName = name; emit userNameChanged(); }
    bool isAdmin() const { return m_isAdmin; }
    QString userPhone() const { return m_userPhone; }
    QString userAddress() const { return m_userAddress; }
    Q_INVOKABLE void saveUserInfo(const QString &phone, const QString &address);
    bool hasOneNetConfig() const;

signals:
    void mqttConnectedChanged();
    void mqttStatusChanged();
    void heartRateChanged();
    void spo2Changed();
    void cabinet1StatusChanged();
    void cabinet2StatusChanged();
    void cartCountChanged();
    void userNameChanged();
    void isAdminChanged();
    void userInfoChanged();
    void logMessage(const QString &msg);

private slots:
    void onPropertyReceived(const QJsonObject &params);

private:
    MqttClient       *m_mqtt;
    DatabaseManager  *m_db;

    bool    m_mqttConnected = false;
    QString m_mqttStatus = "未连接";
    int     m_heartRate = 0;
    int     m_spo2 = 0;
    int     m_cabinet1Status = 0;
    int     m_cabinet2Status = 0;
    QString m_userName = "用户";
    bool    m_isAdmin = false;
    QString m_userPhone = "";
    QString m_userAddress = "";

    static AppState *s_instance;
};

#endif // APPSTATE_H
