#include "mqttclient.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QMessageAuthenticationCode>
#include <QDateTime>
#include <QDebug>

// ────────────────────────────────────────────────────────────
//  MQTT 3.1.1 数据包类型
// ────────────────────────────────────────────────────────────
#define MQTT_CONNECT     0x10
#define MQTT_CONNACK     0x20
#define MQTT_PUBLISH     0x30
#define MQTT_PUBACK      0x40
#define MQTT_SUBSCRIBE   0x82   // type(8) | flags(2)
#define MQTT_SUBACK      0x90
#define MQTT_PINGREQ     0xC0
#define MQTT_PINGRESP    0xD0
#define MQTT_DISCONNECT  0xE0

// ────────────────────────────────────────────────────────────
//  构造 / 析构
// ────────────────────────────────────────────────────────────

MqttClient::MqttClient(QObject *parent)
    : QObject(parent)
    , m_socket(new QTcpSocket(this))
    , m_pingTimer(new QTimer(this))
{
    connect(m_socket, &QTcpSocket::connected,    this, &MqttClient::onConnected);
    connect(m_socket, &QTcpSocket::disconnected, this, &MqttClient::onDisconnected);
    connect(m_socket, &QTcpSocket::readyRead,    this, &MqttClient::onReadyRead);
    connect(m_socket, &QTcpSocket::errorOccurred,this, &MqttClient::onError);
    connect(m_pingTimer, &QTimer::timeout,       this, &MqttClient::onPingTimer);
}

MqttClient::~MqttClient()
{
    disconnectFromHost();
}

// ────────────────────────────────────────────────────────────
//  公开接口
// ────────────────────────────────────────────────────────────

void MqttClient::setOneNetConfig(const QString &productId,
                                 const QString &deviceName,
                                 const QString &deviceKey,
                                 const QString &accessKey)
{
    m_productId  = productId;
    m_deviceName = deviceName;
    m_deviceKey  = deviceKey;
    m_accessKey  = accessKey;
    m_status = "已配置";
    emit statusTextChanged();
}

void MqttClient::connectToHost()
{
    if (m_productId.isEmpty()) {
        emit logMessage("[MQTT] 请先配置 OneNet 三要素");
        return;
    }
    m_status = "正在连接...";
    emit statusTextChanged();
    emit logMessage("[MQTT] 连接 OneNet 标准 MQTT: mqtts.heclouds.com:1883 ...");
    m_socket->connectToHost("mqtts.heclouds.com", 1883);
}

void MqttClient::disconnectFromHost()
{
    m_pingTimer->stop();
    if (m_socket->state() == QAbstractSocket::ConnectedState) {
        QByteArray pkt = buildDisconnectPacket();
        m_socket->write(pkt);
        m_socket->flush();
    }
    m_socket->disconnectFromHost();
    if (m_connected) {
        m_connected = false;
        emit connectedChanged();
    }
    m_status = "未连接";
    emit statusTextChanged();
}

void MqttClient::publishProperty(const QJsonObject &params)
{
    QString topic = QString("$sys/%1/%2/thing/property/post")
                        .arg(m_productId, m_deviceName);

    QJsonObject root;
    root["id"]      = QString::number(QDateTime::currentMSecsSinceEpoch());
    root["version"] = "1.0";
    root["params"]  = params;

    QByteArray payload = QJsonDocument(root).toJson(QJsonDocument::Compact);
    QByteArray pkt = buildPublishPacket(topic, payload);
    m_socket->write(pkt);
    emit logMessage(QString("[MQTT] 发布 → %1").arg(QString::fromUtf8(payload)));
}

// ────────────────────────────────────────────────────────────
//  Socket 回调
// ────────────────────────────────────────────────────────────

void MqttClient::onConnected()
{
    qDebug() << "[MQTT-DEBUG] TCP 连接成功！";
    emit logMessage("[MQTT] TCP 已连接，发送 CONNECT ...");
    QByteArray connectPkt = buildConnectPacket();
    qDebug() << "[MQTT-DEBUG] CONNECT 包大小:" << connectPkt.size() << "bytes";
    qDebug() << "[MQTT-DEBUG] CONNECT 包(hex):" << connectPkt.toHex();
    m_socket->write(connectPkt);
    m_socket->flush();
}

void MqttClient::onDisconnected()
{
    m_pingTimer->stop();
    if (m_connected) {
        m_connected = false;
        emit connectedChanged();
    }
    m_status = "已断开";
    emit statusTextChanged();
    emit logMessage("[MQTT] 连接已断开");
}

void MqttClient::onReadyRead()
{
    m_rxBuffer.append(m_socket->readAll());
    while (m_rxBuffer.size() >= 2) {
        int consumed = 0;
        // 读 Remaining Length
        quint32 rl = 0;
        int multiplier = 1;
        int pos = 1;
        while (pos < m_rxBuffer.size() && pos < 5) {
            quint8 b = (quint8)m_rxBuffer.at(pos);
            rl += (b & 0x7F) * multiplier;
            multiplier *= 128;
            pos++;
            if ((b & 0x80) == 0) break;
        }
        int headerLen = pos;
        int totalLen  = headerLen + (int)rl;
        if (m_rxBuffer.size() < totalLen) break;  // 数据不够

        QByteArray packet = m_rxBuffer.left(totalLen);
        m_rxBuffer.remove(0, totalLen);

        // 分发到对应处理函数
        quint8 type = (quint8)packet.at(0) & 0xF0;
        switch (type) {
        case MQTT_CONNACK:    handleConnAck(packet);    break;
        case MQTT_SUBACK:     handleSubAck(packet);     break;
        case MQTT_PUBLISH:    handlePublish(packet, headerLen); break;
        case MQTT_PINGRESP:   handlePingResp();         break;
        default:
            emit logMessage(QString("[MQTT] 未知包类型 0x%1").arg(type, 2, 16, QChar('0')));
            break;
        }
    }
}

void MqttClient::onError(QAbstractSocket::SocketError err)
{
    Q_UNUSED(err)
    qDebug() << "[MQTT-DEBUG] Socket 错误:" << m_socket->errorString() << "errCode:" << err;
    m_status = "连接失败: " + m_socket->errorString();
    emit statusTextChanged();
    emit logMessage("[MQTT] 错误: " + m_socket->errorString());
}

void MqttClient::onPingTimer()
{
    if (m_connected) {
        QByteArray pkt = buildPingReqPacket();
        m_socket->write(pkt);
    }
}

// ────────────────────────────────────────────────────────────
//  数据包处理
// ────────────────────────────────────────────────────────────

void MqttClient::handleConnAck(const QByteArray &data)
{
    qDebug() << "[MQTT-DEBUG] 收到 CONNACK，大小:" << data.size() << " hex:" << data.toHex();
    if (data.size() < 4) {
        qDebug() << "[MQTT-DEBUG] CONNACK 包太小 (<4 bytes)";
        return;
    }
    quint8 code = (quint8)data.at(3);
    if (code == 0) {
        m_connected = true;
        m_status = "MQTT 已连接";
        emit connectedChanged();
        emit statusTextChanged();
        emit logMessage("[MQTT] CONNACK 成功 ✓");

        // 订阅: 整个 thing 簇（属性和事件下发）
        QString topic = QString("$sys/%1/%2/thing/#")
                            .arg(m_productId, m_deviceName);
        QByteArray subPkt = buildSubscribePacket(m_packetId++, topic);
        m_socket->write(subPkt);

        // 启动心跳 (30秒)
        m_pingTimer->start(30000);
    } else {
        emit logMessage(QString("[MQTT] CONNACK 失败 code=%1").arg(code));
    }
}

void MqttClient::handleSubAck(const QByteArray &data)
{
    Q_UNUSED(data)
    emit logMessage("[MQTT] 订阅成功 ✓");
}

void MqttClient::handlePublish(const QByteArray &data, int headerLen)
{
    // 解析 topic
    int pos = headerLen;
    quint16 topicLen = ((quint8)data.at(pos) << 8) | (quint8)data.at(pos + 1);
    pos += 2;
    QString topic = QString::fromUtf8(data.mid(pos, topicLen));
    pos += topicLen;

    // QoS > 0 时有 packetId，需要发 PUBACK
    quint8 qos = ((quint8)data.at(0) & 0x06) >> 1;
    quint16 incomingPktId = 0;
    if (qos > 0) {
        incomingPktId = ((quint8)data.at(pos) << 8) | (quint8)data.at(pos + 1);
        pos += 2;
        // 发 PUBACK
        if (qos == 1) {
            QByteArray puback;
            puback.append((char)0x40);  // PUBACK
            puback.append((char)0x02);  // Remaining Length = 2
            puback.append((char)(incomingPktId >> 8));
            puback.append((char)(incomingPktId & 0xFF));
            m_socket->write(puback);
            qDebug() << "[MQTT-DEBUG] 发 PUBACK for packetId:" << incomingPktId;
        }
    }

    QByteArray payload = data.mid(pos);
    emit logMessage(QString("[MQTT] ← topic: %1").arg(topic));
    emit logMessage(QString("[MQTT] ← payload: %1").arg(QString::fromUtf8(payload)));
    qDebug() << "[MQTT-DEBUG] PUBLISH topic:" << topic << "payload:" << payload;

    QJsonDocument doc = QJsonDocument::fromJson(payload);
    if (!doc.isObject()) return;
    QJsonObject root = doc.object();

    // ── 属性设置下发 ── $sys/{pid}/{dn}/thing/property/set
    if (topic.endsWith("/thing/property/set")) {
        QString msgId = root.contains("id") ? root["id"].toString() : "0";
        QJsonObject params = root["params"].toObject();

        // 发回 set_reply（OneNet 要求必须回复，否则报"设备响应超时"）
        QString replyTopic = QString("$sys/%1/%2/thing/property/set_reply")
                                 .arg(m_productId, m_deviceName);
        QJsonObject reply;
        reply["id"]   = msgId;
        reply["code"] = 200;
        reply["msg"]  = "success";
        QByteArray replyPayload = QJsonDocument(reply).toJson(QJsonDocument::Compact);
        QByteArray replyPkt = buildPublishPacket(replyTopic, replyPayload);
        m_socket->write(replyPkt);
        emit logMessage("[MQTT] → 回复 set_reply ✓");
        qDebug() << "[MQTT-DEBUG] 发 set_reply: " << replyPayload;

        // 通知 AppState 更新属性
        if (!params.isEmpty()) {
            emit propertyReceived(params);
        }
        return;
    }

    // ── 其他 thing 消息（post 回执等） ──
    if (root.contains("params")) {
        emit propertyReceived(root["params"].toObject());
    }
}

void MqttClient::handlePingResp()
{
    // 静默
}

// ────────────────────────────────────────────────────────────
//  数据包构造
// ────────────────────────────────────────────────────────────

QByteArray MqttClient::encodeUtf8(const QString &s)
{
    QByteArray utf8 = s.toUtf8();
    QByteArray out;
    out.append((char)(utf8.size() >> 8));
    out.append((char)(utf8.size() & 0xFF));
    out.append(utf8);
    return out;
}

QByteArray MqttClient::encodeRemainingLength(int len)
{
    QByteArray out;
    while (len > 0) {
        quint8 b = len % 128;
        len /= 128;
        if (len > 0) b |= 0x80;
        out.append((char)b);
    }
    return out;
}

QByteArray MqttClient::buildConnectPacket()
{
    // ── OneNet 标准 MQTT Token 认证 ──────────
    // 参考 SDK 源码 + 官方文档: open.iot.10086.cn/doc/aiot/
    // 格式: version=2018-10-31&res=products%2F{pid}%2Fdevices%2F{dn}&et={unix_ts}&method=md5&sign={url_encoded_base64}
    QString version = "2018-10-31";

    //   res = products/{pid}/devices/{dn}  — 含设备名
    QString res = QString("products/%1/devices/%2").arg(m_productId, m_deviceName);

    //   et = Unix 时间戳（秒）
    qint64 et = QDateTime::currentDateTimeUtc().addSecs(86400 * 365).toSecsSinceEpoch();
    QString method = "md5";

    // 待签名字符串: et\nmethod\nres\nversion  (每行 \n 分隔)
    QString signStr = QString::number(et) + "\n" + method + "\n" + res + "\n" + version;

    //   产品 access_key 需要 Base64 解码后再作为 HMAC key
    QByteArray keyBytes = QByteArray::fromBase64(m_accessKey.toUtf8());

    // HMAC-MD5(key=keyBytes, data=signStr) → 标准 Base64（保留 == 尾部填充）
    QByteArray sign = QMessageAuthenticationCode::hash(
        signStr.toUtf8(), keyBytes,
        QCryptographicHash::Md5
    ).toBase64();  // 标准 Base64，保留 == 填充

    //   对整个 sign 做 URL 编码（不是手工替换 +/= 字符）
    QString signEncoded = QString::fromUtf8(sign.toPercentEncoding());
    QString resEncoded  = QString::fromUtf8(res.toUtf8().toPercentEncoding());

    QString token = QString("version=%1&res=%2&et=%3&method=%4&sign=%5")
                        .arg(version, resEncoded, QString::number(et), method, signEncoded);

    // ═══════════════════════════════════════════
    // 调试输出：打印完整 Token 及关键参数
    // ═══════════════════════════════════════════
    qDebug() << "[MQTT-DEBUG] ====== OneNet Token 调试信息 ======";
    qDebug() << "[MQTT-DEBUG] productId :" << m_productId;
    qDebug() << "[MQTT-DEBUG] deviceName:" << m_deviceName;
    qDebug() << "[MQTT-DEBUG] method    :" << method;
    qDebug() << "[MQTT-DEBUG] et (UTC) :" << et;
    qDebug() << "[MQTT-DEBUG] res (raw) :" << res;
    qDebug() << "[MQTT-DEBUG] res (enc) :" << resEncoded;
    qDebug() << "[MQTT-DEBUG] sign (B64):" << QString::fromUtf8(sign);
    qDebug() << "[MQTT-DEBUG] sign (enc):" << signEncoded;
    qDebug() << "[MQTT-DEBUG] ====== 完整 Token ======";
    qDebug().noquote() << "[MQTT-DEBUG]" << token;
    qDebug() << "[MQTT-DEBUG] ====== Token 长度:" << token.length() << "======";
    // 同时输出到 UI 日志（截断，避免 Android logcat 单行限制）
    emit logMessage("[MQTT] 生成Token: productId=" + m_productId + " device=" + m_deviceName);
    emit logMessage("[MQTT] Token(前80字符): " + token.left(80) + "...");
    emit logMessage("[MQTT] Token 完整内容请查看 Android Studio Logcat (过滤 MqttClient)");

    // Variable Header
    QByteArray vh;
    vh.append(encodeUtf8("MQTT"));
    vh.append((char)4);                  // MQTT 3.1.1
    vh.append((char)0xC2);               // Flags: Username(0x80)|Password(0x40)|CleanSession(0x02)
    vh.append((char)0); vh.append((char)60); // Keep Alive = 60s

    // Payload
    QByteArray payload;
    payload.append(encodeUtf8(m_deviceName));   // Client ID
    payload.append(encodeUtf8(m_productId));    // Username
    payload.append(encodeUtf8(token));          // Password = OneNet token

    QByteArray variableAndPayload = vh + payload;
    QByteArray rl = encodeRemainingLength(variableAndPayload.size());

    QByteArray pkt;
    pkt.append((char)MQTT_CONNECT);
    pkt.append(rl);
    pkt.append(variableAndPayload);
    return pkt;
}

QByteArray MqttClient::buildPublishPacket(const QString &topic, const QByteArray &payload)
{
    QByteArray vh;
    vh.append(encodeUtf8(topic));
    // QoS 0, no packetId

    QByteArray data = vh + payload;
    QByteArray rl = encodeRemainingLength(data.size());

    QByteArray pkt;
    pkt.append((char)MQTT_PUBLISH);  // QoS 0
    pkt.append(rl);
    pkt.append(data);
    return pkt;
}

QByteArray MqttClient::buildSubscribePacket(quint16 packetId, const QString &topic)
{
    QByteArray vh;
    vh.append((char)(packetId >> 8));
    vh.append((char)(packetId & 0xFF));

    QByteArray payload;
    payload.append(encodeUtf8(topic));
    payload.append((char)0);  // QoS 0

    QByteArray data = vh + payload;
    QByteArray rl = encodeRemainingLength(data.size());

    QByteArray pkt;
    pkt.append((char)MQTT_SUBSCRIBE);
    pkt.append(rl);
    pkt.append(data);
    return pkt;
}

QByteArray MqttClient::buildPingReqPacket()
{
    return QByteArray::fromRawData("\xC0\x00", 2);
}

QByteArray MqttClient::buildDisconnectPacket()
{
    return QByteArray::fromRawData("\xE0\x00", 2);
}
