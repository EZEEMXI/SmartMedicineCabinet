#ifndef MQTTCLIENT_H
#define MQTTCLIENT_H

#include <QObject>
#include <QTcpSocket>
#include <QTimer>
#include <QJsonObject>

/**
 * 轻量 MQTT 3.1.1 客户端
 *
 * 连接 OneNet 标准 MQTT/物模型平台：
 *   Broker:  mqtts.heclouds.com:1883 (非 TLS)
 *   认证:    productId / deviceName / accessKey → HMAC-MD5 token
 *   算法:    key = base64_decode(accessKey)
 *           org = "{et}\n{method}\n{res}\n{version}"
 *           sign = HMAC-MD5(key, org) → Base64 → URL 编码
 *   格式:    token = version=2018-10-31&res=products%2F{pid}%2Fdevices%2F{dn}&et={unix_ts}&method=md5&sign={url_encoded}
 *   Publish: $sys/{pid}/{dn}/thing/property/post
 *   Sub:     $sys/{pid}/{dn}/thing/#
 */
class MqttClient : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool connected READ connected NOTIFY connectedChanged)
    Q_PROPERTY(QString statusText READ statusText NOTIFY statusTextChanged)

public:
    explicit MqttClient(QObject *parent = nullptr);
    ~MqttClient();

    bool connected() const { return m_connected; }
    QString statusText() const { return m_status; }

    // ── 配置 ────────────────────────────────
    Q_INVOKABLE void setOneNetConfig(const QString &productId,
                                     const QString &deviceName,
                                     const QString &deviceKey,
                                     const QString &accessKey);
    Q_INVOKABLE void connectToHost();
    Q_INVOKABLE void disconnectFromHost();

    // ── 发布 ────────────────────────────────
    Q_INVOKABLE void publishProperty(const QJsonObject &params);

signals:
    void connectedChanged();
    void statusTextChanged();
    void propertyReceived(const QJsonObject &params);  // 收到设备上报
    void logMessage(const QString &msg);

private slots:
    void onConnected();
    void onDisconnected();
    void onReadyRead();
    void onError(QAbstractSocket::SocketError err);
    void onPingTimer();

private:
    // ── MQTT 包构造 ─────────────────────────
    QByteArray buildConnectPacket();
    QByteArray buildPublishPacket(const QString &topic, const QByteArray &payload);
    QByteArray buildSubscribePacket(quint16 packetId, const QString &topic);
    QByteArray buildPingReqPacket();
    QByteArray buildDisconnectPacket();

    // ── MQTT 包解析 ─────────────────────────
    void processPacket();
    void handleConnAck(const QByteArray &data);
    void handleSubAck(const QByteArray &data);
    void handlePublish(const QByteArray &data, int headerLen);
    void handlePingResp();

    // ── 工具 ────────────────────────────────
    static QByteArray encodeRemainingLength(int len);
    static QByteArray encodeUtf8(const QString &s);

    QTcpSocket  *m_socket;
    QTimer      *m_pingTimer;
    bool         m_connected = false;
    QString      m_status = "未连接";

    // OneNet 四要素
    QString m_productId;
    QString m_deviceName;
    QString m_deviceKey;    // 设备密钥
    QString m_accessKey;    // 产品 access_key（Token 签名用）

    quint16 m_packetId = 1;
    QByteArray m_rxBuffer;
};

#endif // MQTTCLIENT_H
