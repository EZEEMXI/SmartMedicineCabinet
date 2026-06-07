// module/medicine_db.h
#ifndef __MEDICINE_DB_H
#define __MEDICINE_DB_H

#include "main.h"

#define MEDICINE_COUNT  5
#define MEDICINE_CODE_LEN  16
#define MEDICINE_DESC_LEN  128

typedef struct {
    char code[MEDICINE_CODE_LEN];       // 条形码数字
    char name[32];                      // 药品名
    char desc[MEDICINE_DESC_LEN];       // 描述文字
    uint8_t name_gb[32];                // 药品名 GB2312 编码
    uint8_t name_gb_len;
    uint8_t desc_gb[256];               // 描述 GB2312 编码
    uint8_t desc_gb_len;
} Medicine_t;

extern const Medicine_t medicine_db[MEDICINE_COUNT];

uint8_t Medicine_FindByCode(const char *code);

#endif
