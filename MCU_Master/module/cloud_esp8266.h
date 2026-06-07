/**
 ************************************************************
 * @file    cloud_esp8266.h
 * @brief   ESP8266 MQTT AT 命令接口
 ************************************************************
 */
#ifndef __CLOUD_ESP8266_H
#define __CLOUD_ESP8266_H

#include "main.h"
#include <stdint.h>

void    ESP8266_Init(void);
void    ESP8266_ConnectMQTT(void);
uint8_t  ESP8266_IsMQTTConnected(void);
void    ESP8266_PublishData(const char* json_str);
void    ESP8266_Process(void);
void    ESP8266_DisconnectMQTT(void);

#endif
