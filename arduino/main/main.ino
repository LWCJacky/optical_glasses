//警告除非你知道你在做什麼，否則別改這頁的內容需要自定義參數請修改config.h

#include "config.h"


#include <Keypad.h>
#include <U8g2lib.h>
#include <Wire.h>
#include "stdint.h"
#define I2C_SDA 21
#define I2C_SCL 22


int RightEyePosition[] = { Location_1, Location_2, Location_3, Location_4,
                           Location_5, Location_6, Location_7, Location_8,
                           Location_9, Location_10, Location_11, Location_12,
                           Location_13, Location_14, Location_15, Location_16,
                           Location_17, Location_18, Location_19, Location_20 };
int RightEyeLowPosition[] = { low_Location_1, low_Location_2, low_Location_3, low_Location_4,
                              low_Location_5, low_Location_6, low_Location_7, low_Location_8,
                              low_Location_9, low_Location_10, low_Location_11, low_Location_12,
                              low_Location_13, low_Location_14, low_Location_15, low_Location_16,
                              low_Location_17, low_Location_18, low_Location_19, low_Location_20 };
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/U8X8_PIN_NONE);

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, rows, cols);

//菜單狀態請勿修改//
bool show_menu_stest = true;
int8_t pages = 1;
int pulse = 0;

void motor_state(int speed  //速度
                 ,
                 bool DIR_s  //方向
                 ,
                 int pulse_s  //脈波數量
                 ,
                 bool mode  //模式 true 位置控制 /false 增量控制
) {
  digitalWrite(DIR, DIR_s);  //true正轉,false反轉
  Serial.println(-10 - 10);
  if (mode) {
    Serial.print(pulse_s);
    Serial.print("-");
    Serial.println(pulse);


    int err = pulse_s - pulse;
    Serial.println(err);
    if (err > 0) {
      DIR_s = 1;
      digitalWrite(DIR, HIGH);
    } else {
      DIR_s = 0;
      digitalWrite(DIR, LOW);
    }
    err = abs(err);
    for (int i = 0; i < err; i++) {
      digitalWrite(STEP, HIGH);
      delayMicroseconds(speed);
      digitalWrite(STEP, LOW);
      delayMicroseconds(speed);
      if (DIR_s) {
        pulse++;
      } else {
        pulse--;
      }
    }
  } else {
    for (int i = 0; i < pulse_s; i++) {
      digitalWrite(STEP, HIGH);
      delayMicroseconds(speed);
      digitalWrite(STEP, LOW);
      delayMicroseconds(speed);
      if (DIR_s) {
        pulse++;
      } else {
        pulse--;
      }
    }
  }
}




void setup() {


  Serial.begin(115200);
  pinMode(STEP, OUTPUT);
  pinMode(DIR, OUTPUT);


  delay(3000);
  u8g2.begin();
  u8g2.enableUTF8Print();
  u8g2.setFont(u8g2_font_unifont_t_chinese1);  // use chinese2 for all the glyphs of "你好世界"
  u8g2.setFontDirection(0);
  u8g2.clearBuffer();
  u8g2.setCursor(0, 40);
  u8g2.print("歡迎使用光學眼鏡");
  u8g2.sendBuffer();
  delay(1000);
  show_menu();
  // run();
}
int get_key() {
  char key = keypad.getKey();
  int int_key = -1;
  if (key != NO_KEY) {
    // Serial.println(key);
    switch (key) {
      case '1':
        int_key = 1;
        break;
      case '2':
        int_key = 2;
        break;
      case '3':
        int_key = 3;
        break;
      case '4':
        int_key = 4;
        break;
      case '5':
        int_key = 5;
        break;
      case '6':
        int_key = 6;
        break;
      case '7':
        int_key = 7;
        break;
      case '8':
        int_key = 8;
        break;
      case '9':
        int_key = 9;
        break;
      case '0':
        int_key = 0;
        break;
    }
    // Serial.println(int_key);

    return int_key;
  }
}
void loop() {
  int8_t key = get_key();
  if (key > 0) {
    OptionsUpdate(key);
  }
}

void ReturnToZero(int8_t options) {
  u8g2.clearBuffer();
  u8g2.setCursor(0, 15);
  u8g2.print("1.正轉");
  u8g2.setCursor(0, 35);
  u8g2.print("2.反轉");
  u8g2.setCursor(0, 55);
  u8g2.print("3.返回");
  u8g2.sendBuffer();
  while (1) {
    int key = get_key();
    switch (key) {
      case 1:
        motor_state(800, true, 20, 0);
        break;
      case 2:
        motor_state(800, false, 20, 0);
        break;
      case 3:
        pulse = 0;
        show_menu();
        return;
    }
  }
}
void OptionsUpdate(int8_t options) {  //選項輸入更新處理
  switch (options) {
    case 1:
      if (pages == 1) {
        ReturnToZero(options);  //手動歸零
      } else if (pages == 2) {
        Text(options);  //旋轉角度測試
      }
      break;
    case 2:
      if (pages == 1) {
        run();  //執行主程式
      } else if (pages == 2) {
      }
      break;
    case 3:
      if (pages >= 2) {
        pages = 1;
      } else {
        pages++;
      }
      show_menu();
      break;
  }
}
void Text(int8_t options) {  //旋轉角度測試
  u8g2.clearBuffer();
  u8g2.setCursor(0, 15);
  u8g2.print("1.左眼");
  u8g2.setCursor(0, 35);
  u8g2.print("2.右眼");
  u8g2.setCursor(0, 55);
  u8g2.print("3.返回");
  u8g2.sendBuffer();
  while (1) {
    int key = get_key();
    switch (key) {
      case 1:
        Text_1();
        break;
      case 2:
        Text_1();
        break;
      case 3:
        show_menu();
        return;
    }
  }
}
void Text_1() {
  u8g2.clearBuffer();
  u8g2.setCursor(0, 15);
  u8g2.print("1.正轉");
  u8g2.setCursor(0, 35);
  u8g2.print("2.反轉");
  u8g2.setCursor(0, 55);
  u8g2.print("3.返回");
  u8g2.sendBuffer();
  while (1) {
    int key = get_key();
    u8g2.clearBuffer();
    u8g2.setCursor(0, 15);
    u8g2.print(pulse);
    u8g2.sendBuffer();
    switch (key) {
      case 1:
        motor_state(800, true, 100, 0);
        break;
      case 2:
        motor_state(800, false, 100, 0);
        break;
      case 3:
        show_menu();
        return;
    }
  }
}
void Correction() {  //此功能以停用
  bool state = true;
  while (1) {
    if (!digitalRead(dia) && state) {
      for (int i = 0; i <= 21; i++) {
        digitalWrite(STEP, HIGH);
        delayMicroseconds(800);
        digitalWrite(STEP, LOW);
        delayMicroseconds(800);
      }
      state = false;
    }
    if (digitalRead(dia) && !state) {
      state = true;
    }
  }
}


void show_menu() {  //顯示菜單
  if (pages == 1) {
    u8g2.clearBuffer();
    u8g2.setCursor(0, 15);
    u8g2.print("1.手動歸零");
    u8g2.setCursor(0, 35);
    u8g2.print("2.執行主程序");
    u8g2.setCursor(0, 55);
    u8g2.print("3.下一頁");
    u8g2.sendBuffer();
  } else if (pages == 2) {
    u8g2.clearBuffer();
    u8g2.setCursor(0, 15);
    u8g2.print("1.TEXT");
    u8g2.setCursor(0, 35);
    u8g2.print("2.XXXXXX");
    u8g2.setCursor(0, 55);
    u8g2.print("3.下一頁");
    u8g2.sendBuffer();
  }
}

void run() {  //執行主程式
  String stringThree;
  int8_t R = 0;
  int8_t L = 0;
  bool SelectMode;  //TRUE=雙眼參數同步/FALSE=雙眼設定獨立參數
  while (1) {
    char key = keypad.getKey();
    bool state = false;
    u8g2.clearBuffer();
    u8g2.setCursor(0, 15);
    u8g2.print("1.one");
    u8g2.setCursor(0, 35);
    u8g2.print("2.two");
    u8g2.setCursor(0, 55);
    u8g2.print("3.返回");
    u8g2.sendBuffer();
    switch (key) {
      case '1':
        SelectMode = true;
        state = true;
        break;
      case '2':
        SelectMode = false;
        state = true;
        break;
      case '3':
        show_menu();
        return;
    }
    if (state) {
      break;
    }
  }
  int8_t counter = 0;
  while (1) {
    char key = keypad.getKey();

    u8g2.clearBuffer();
    u8g2.setCursor(0, 15);
    if (SelectMode) {
      u8g2.print("左右眼 INPUT");
    } else if (counter == 0) {
      u8g2.print("右眼 INPUT");
    } else {
      u8g2.print("左眼 INPUT");
    }

    // u8g2.sendBuffer();

    if (key != '*' && key != '#' && key != 0) {

      stringThree += key;

      Serial.println(stringThree);
    }
    u8g2.setCursor(15, 35);
    u8g2.print(stringThree);
    u8g2.sendBuffer();
    if (stringThree.length() == 2) {
      if (SelectMode) {
        R = stringThree.toInt();
        L = stringThree.toInt();
        Serial.print("R=");
        Serial.println(R);
        Serial.print("L=");
        Serial.println(L);
        delay(1000);
        show_menu();
        return;
      } else if (counter == 0) {
        R = stringThree.toInt();
        stringThree = "";
        counter++;
      } else if (counter == 1) {
        L = stringThree.toInt();
        Serial.print("R=");
        Serial.println(R);
        Serial.print("L=");
        Serial.println(L);
        delay(1000);
        show_menu();
        return;
      }
    }
    switch (key) {
      // case 1:
      //   motor_state(800, true, 100, 0);
      //   break;
      // case 2:
      //   motor_state(800, false, 100, 0);
      //   break;
      case 3:
        show_menu();
        return;
    }
  }
  // motor_state( 800  //速度
  //             ,
  //             0  //方向
  //             ,
  //             Location_20  //脈波數量
  //             ,
  //             true  //模式 true 位置控制 /false 增量控制
  // );
  // delay(1000);
  // motor_state( 800  //速度
  //             ,
  //             0  //方向
  //             ,
  //             0  //脈波數量
  //             ,
  //             true  //模式 true 位置控制 /false 增量控制
  // );
  // delay(1000);
  // motor_state( 800  //速度
  //             ,
  //             0  //方向
  //             ,
  //             -Location_20  //脈波數量
  //             ,
  //             true  //模式 true 位置控制 /false 增量控制
  // );
  // delay(1000);
  //  motor_state( 800  //速度
  //             ,
  //             0  //方向
  //             ,
  //             0  //脈波數量
  //             ,
  //             true  //模式 true 位置控制 /false 增量控制
  // );
}