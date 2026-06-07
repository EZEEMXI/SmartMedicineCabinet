/**
 ************************************************************
 * @file    cloud_proto.h
 * @brief   云端数据上报接口
 ************************************************************
 */
#ifndef __CLOUD_PROTO_H
#define __CLOUD_PROTO_H

#include "main.h"
#include <stdint.h>

void CloudProto_ReportOpen(uint8_t cabinet_id, uint8_t method);
void CloudProto_ReportTaken(uint8_t cabinet_id);
void CloudProto_ReportHeartRate(uint8_t heart_rate);
void CloudProto_ReportSpO2(uint8_t spo2);
void CloudProto_ReportMedicine(const char* barcode);
void CloudProto_ReportUser(uint8_t user_type);

void    CloudProto_Flush(void);       /* 发送队列中下一条消息 */
uint8_t CloudProto_HasPending(void);  /* 队列是否还有待发送 */

#endif
