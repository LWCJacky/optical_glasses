

#include "config.h"


#include <Keypad.h>
#include <U8g2lib.h>
#include <Wire.h>
#include "stdint.h"
#define I2C_SDA 21
#define I2C_SCL 22
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
  Serial.println("Spinning Clockwise...");
  if (mode) {
    int err = pulse_s - pulse;
    if (err > 0) {
      digitalWrite(DIR, HIGH);
    } else {
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
    Serial.println(key);
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
void OptionsUpdate(int8_t options) {
  switch (options) {
    case 1:
      if (pages == 1) {
        ReturnToZero(options);  //手動歸零
      }
      break;
    case 2:
      break;
    case 3:
      if (pages >= 2) {
        pages = 1;
      } else {
        pages ++;
      }
      show_menu();
      break;
  }
}
void Correction() {
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

void run() {
  digitalWrite(DIR, HIGH);
  Serial.println("Spinning Clockwise...");

  for (int i = 0; i < steps_per_rev; i++) {
    digitalWrite(STEP, HIGH);
    delayMicroseconds(800);
    digitalWrite(STEP, LOW);
    delayMicroseconds(800);
  }
  delay(1000);

  digitalWrite(DIR, LOW);
  Serial.println("Spinning Anti-Clockwise...");

  for (int i = 0; i <= steps_per_rev; i++) {
    digitalWrite(STEP, HIGH);
    delayMicroseconds(800);
    digitalWrite(STEP, LOW);
    delayMicroseconds(800);
  }
  delay(1000);
}