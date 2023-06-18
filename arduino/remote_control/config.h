#ifndef CONFIG_H
#define CONFIG_H
#include <U8g2lib.h>
#define ESPNOW_CHANNEL 1
// uint8_t receiverMacAddress[] = {0xC0,0x49,0xEF,0xBE,0xCE,0x2C};  // 替換為 Sender 的 MAC 位址
uint8_t receiverMacAddress[] ={0xC0,0x49,0xEF,0xBB,0x7E,0x58};  // 替換為 Sender 的 MAC 位址
// 定義配置參數
const byte rows = 4; //four rows
const byte cols = 3; //three columns
char keys[rows][cols] = {
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'},
  {'*','0','#'}
};
byte rowPins[rows] = {27, 26, 25, 33}; //connect to the row pinouts of the keypad
byte colPins[cols] = {13,12,14}; //connect to the column pinouts of the keypad



#endif