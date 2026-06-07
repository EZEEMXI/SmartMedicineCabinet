/**
 ************************************************************
 * @file    cloud_proto.c
 * @brief   云端数据协议 - OneNet MQTT JSON 格式
 *
 * 设计思路：
 *   - ReportXXX() 只把 JSON 写入缓冲队列（不阻塞）
 *   - 主循环调用 CloudProto_Flush() 逐条发送
 *   - UART2 分时复用：扫码时攒数据，切到 WiFi 后集中上报
 ************************************************************
 */
#include "cloud_proto.h"
#include "cloud_esp8266.h"
#include <stdio.h>
#include <string.h>

/* ===== 消息队列 ===== */
#define CLOUD_QUEUE_SIZE  8       /* 最多缓存 8 条待上报消息 */
#define CLOUD_JSON_MAX    256     /* 每条 JSON 最大长度     */

typedef struct {
    char json[CLOUD_JSON_MAX];
} CloudMsg_t;

static CloudMsg_t cloud_queue[CLOUD_QUEUE_SIZE];
static uint8_t    cloud_wr  = 0;  /* 写指针 */
static uint8_t    cloud_rd  = 0;  /* 读指针 */
static uint8_t    cloud_cnt = 0;  /* 待发送数量 */

/* 消息计数器（每次上报自增，用作消息ID） */
static uint32_t msg_id = 0;

/* ---- 内部：把 JSON 放入队列 ---- */
static uint8_t cloud_enqueue(const char* json)
{
    if (cloud_cnt >= CLOUD_QUEUE_SIZE)
        return 0;   /* 队列满，丢弃（极端情况） */

    strncpy(cloud_queue[cloud_wr].json, json, CLOUD_JSON_MAX - 1);
    cloud_queue[cloud_wr].json[CLOUD_JSON_MAX - 1] = '\0';
    cloud_wr = (cloud_wr + 1) % CLOUD_QUEUE_SIZE;
    cloud_cnt++;
    return 1;
}

/* ---- 公开接口 ---- */

/**
 * 开柜上报
 */
void CloudProto_ReportOpen(uint8_t cabinet_id, uint8_t method)
{
    const char* method_str;
    switch (method) {
        case 1: method_str = "qrcode"; break;
        case 2: method_str = "voice";  break;
        default:method_str = "unknown";break;
    }

    char json[CLOUD_JSON_MAX];
    sprintf(json,
        "{\"id\":%lu,\"dp\":{"
        "\"door_status\":[{\"v\":1}],"
        "\"open_method\":[{\"v\":\"%s\"}],"
        "\"cabinet_id\":[{\"v\":%d}]"
        "}}",
        ++msg_id, method_str, cabinet_id);

    cloud_enqueue(json);
}

/**
 * 关门/取药上报
 */
void CloudProto_ReportTaken(uint8_t cabinet_id)
{
    char json[CLOUD_JSON_MAX];
    sprintf(json,
        "{\"id\":%lu,\"dp\":{"
        "\"door_status\":[{\"v\":0}],"
        "\"cabinet_id\":[{\"v\":%d}],"
        "\"medicine_taken\":[{\"v\":1}]"
        "}}",
        ++msg_id, cabinet_id);

    cloud_enqueue(json);
}

/**
 * 心率上报
 */
void CloudProto_ReportHeartRate(uint8_t heart_rate)
{
    char json[CLOUD_JSON_MAX];
    sprintf(json,
        "{\"id\":%lu,\"dp\":{"
        "\"heart_rate\":[{\"v\":%d}]"
        "}}",
        ++msg_id, heart_rate);

    cloud_enqueue(json);
}

/**
 * 血氧上报
 */
void CloudProto_ReportSpO2(uint8_t spo2)
{
    char json[CLOUD_JSON_MAX];
    sprintf(json,
        "{\"id\":%lu,\"dp\":{"
        "\"spo2\":[{\"v\":%d}]"
        "}}",
        ++msg_id, spo2);

    cloud_enqueue(json);
}

/**
 * 扫码药品上报
 */
void CloudProto_ReportMedicine(const char* barcode)
{
    char json[CLOUD_JSON_MAX];
    sprintf(json,
        "{\"id\":%lu,\"dp\":{"
        "\"medicine_code\":[{\"v\":\"%s\"}]"
        "}}",
        ++msg_id, barcode);

    cloud_enqueue(json);
}

/**
 * 用户识别上报
 */
void CloudProto_ReportUser(uint8_t user_type)
{
    const char* type_str = (user_type == 2) ? "courier" : "normal";

    char json[CLOUD_JSON_MAX];
    sprintf(json,
        "{\"id\":%lu,\"dp\":{"
        "\"user_type\":[{\"v\":\"%s\"}]"
        "}}",
        ++msg_id, type_str);

    cloud_enqueue(json);
}

/**
 * 发送队列中下一条待上报消息
 *
 * 每次调用只发一条（避免阻塞主循环太久），
 * 由主循环反复调用直到队列清空。
 */
void CloudProto_Flush(void)
{
    if (cloud_cnt == 0) return;

    ESP8266_PublishData(cloud_queue[cloud_rd].json);
    cloud_rd = (cloud_rd + 1) % CLOUD_QUEUE_SIZE;
    cloud_cnt--;
}

/**
 * 队列中是否还有待发送消息
 */
uint8_t CloudProto_HasPending(void)
{
    return (cloud_cnt > 0) ? 1 : 0;
}
