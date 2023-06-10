#ifndef CONFIG_H
#define CONFIG_H
#include <U8g2lib.h>

// 定義配置參數
#define DIR 12
#define STEP 14
#define dia 4
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
#define Location_20 =0

#endif