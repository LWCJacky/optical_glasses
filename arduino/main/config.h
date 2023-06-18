#ifndef CONFIG_H
#define CONFIG_H
#include <U8g2lib.h>
#define ESPNOW_CHANNEL 1
uint8_t senderMacAddress[] = {0xC0,0x49,0xEF,0xBB,0x7E,0x58};  // 替換為 Sender 的 MAC 位址

// 定義配置參數
// 步進馬達引腳配置
#define DIR_PIN_1 12
#define STEP_PIN_1 14
#define EN_1 13

#define DIR_PIN_2 4
#define STEP_PIN_2 16
#define EN_2 17


// #define DIR 12
// #define STEP 14
#define STEPS_PER_REV 64   // 步進馬達每個完整旋轉的步數
#define steps_per_rev 2048*3.84
const byte rows = 4; //four rows
const byte cols = 3; //three columns
char keys[rows][cols] = {
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'},
  {'*','0','#'}
};
byte rowPins[rows] = {27, 26, 25, 33}; //connect to the row pinouts of the keypad
byte colPins[cols] = {5,18,19}; //connect to the column pinouts of the keypad

//位置定義
#define Location_20 2000
#define Location_19 2000
#define Location_18 2000
#define Location_17 2000
#define Location_16 1400
#define Location_15 1100
#define Location_14 800
#define Location_13 400
#define Location_12 400
#define Location_11 400
#define Location_10 400
#define Location_9 600
#define Location_8 400
#define Location_7 400
#define Location_6 200
#define Location_5 200
#define Location_4 200
#define Location_3 200
#define Location_2 200
#define Location_1 200
//負值
#define low_Location_1 200
#define low_Location_2 200
#define low_Location_3 200
#define low_Location_4 200
#define low_Location_5 200
#define low_Location_6 200
#define low_Location_7 200
#define low_Location_8 200
#define low_Location_9 200
#define low_Location_10 200
#define low_Location_11 200
#define low_Location_12 200
#define low_Location_13 200
#define low_Location_14 200
#define low_Location_15 200
#define low_Location_16 200
#define low_Location_17 200
#define low_Location_18 200
#define low_Location_19 200
#define low_Location_20 200
#endif