#ifndef __DEV_SCANNER_H
#define __DEV_SCANNER_H

#include "main.h"

void Dev_Scanner_Init(void);
void Dev_Scanner_Process(void);
uint8_t Dev_Scanner_IsDone(void);
const char* Dev_Scanner_GetCode(void);
void Dev_Scanner_Clear(void);

#endif
