//C0:49:EF:BE:CE:2C
#include "config.h"
#include <Keypad.h>
#include <esp_now.h>
#include <WiFi.h>
#include <U8g2lib.h>
#define I2C_SDA 21
#define I2C_SCL 22
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, rows, cols);
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/U8X8_PIN_NONE);
bool show_menu_stest = true;
int8_t pages = 1;
// int pulse = 0;

// 傳輸資料的結構體
typedef struct struct_message {
  int Motor_number;  //0=同步 1＝1號馬達/ 2=二號馬達/3 =執行主程式
  int mode;          //模式 0=位置模式 //1=增量控制//Motor_number=3時mode＝1退出主程式
  int pulse;         //mode=0 指定移動的位置 //mode=1 移動的距離
  bool dir;          //方向 //mode=0 此方法無效 //mode=1 0=正轉/1=反轉 //此方法以機構指針方向定義非馬達方向
  int speed;         //速度
  bool res;          // 0＝正常狀態/1＝歸零
} struct_message;
struct_message myData;
esp_now_peer_info_t peerInfo;
// 傳送資料的回呼函式
// callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}





void setup() {
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
  Serial.begin(115200);
  // 初始化 ESP32 Wi-Fi
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();

  // 獲取並顯示 MAC 位址
  uint8_t mac[6];
  WiFi.macAddress(mac);
  Serial.print("MAC Address: ");
  for (int i = 0; i < 6; ++i) {
    Serial.print(mac[i], HEX);
    if (i < 5) {
      Serial.print(":");
    }
  }
  Serial.println();


  // 初始化 ESP32 Wi-Fi
  WiFi.mode(WIFI_STA);


  // 初始化 ESPNOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer");
    return;
  }

  // 設定 ESPNOW 傳送資料的回呼函式
  esp_now_register_send_cb(OnDataSent);


  // 設定 ESPNOW 對等裝置（Receiver）
  memcpy(peerInfo.peer_addr, receiverMacAddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;
  // Add peer
  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer");
    return;
  }
}

void loop() {
  // put your main code here, to run repeatedly:

  int8_t key = get_key();
  if (key > 0) {
    OptionsUpdate(key);
  }
}




void espnow_control(int Motor_number, int mode, int pulse, bool dir, int speed, bool res) {
  // Set values to send
  // strcpy(myData.a, "THIS IS A CHAR");
  // myData.b = random(1,20);
  // myData.c = 1.2;
  // myData.d = false;
  myData.Motor_number = Motor_number;
  myData.mode = mode;    //模式 0=位置模式 //1=增量控制
  myData.pulse = pulse;  //mode=0 指定移動的位置 //mode=1 移動的距離
  myData.dir = dir;      //方向 //mode=0 此方法無效 //mode=1 0=正轉/1=反轉 //此方法以機構指針方向定義非馬達方向
  myData.speed = speed;  //速度
  myData.res = res;      // 0＝正常狀態/1＝歸零
  // Send message via ESP-NOW
  esp_err_t result = esp_now_send(receiverMacAddress, (uint8_t *)&myData, sizeof(myData));

  if (result == ESP_OK) {
    Serial.println("Sent with success");
  } else {
    Serial.println("Error sending the data");
  }
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



int EyeSelection() {
  u8g2.clearBuffer();
  u8g2.setCursor(0, 15);
  u8g2.print("1.左眼");
  u8g2.setCursor(0, 35);
  u8g2.print("2.右眼");
  u8g2.sendBuffer();
  while (1) {
    int key = get_key();
    switch (key) {
      case 1:
        return 1;
      case 2:
        return 2;
    }
  }
}

void ReturnToZero(int8_t options) {
  int eye = EyeSelection();
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
        espnow_control(eye, 1, 20, 0, 200, 0);
        break;
      case 2:
        espnow_control(eye, 1, 20, 1, 200, 0);
        break;
      case 3:
        // pulse = 0;
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
    u8g2.print("abc");
    u8g2.sendBuffer();
    switch (key) {
      case 1:
        // motor_state(800, true, 100, 0);
        break;
      case 2:
        // motor_state(800, false, 100, 0);
        break;
      case 3:
        show_menu();
        return;
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
  int R = 0;
  int L = 0;
  int R_speed = 0;
  int L_speed = 0;

  bool SelectMode;  //TRUE=雙眼參數同步/FALSE=雙眼設定獨立參數
  //選擇模式
  // while (1) {
  // char key = keypad.getKey();
  // bool state = false;
  // u8g2.clearBuffer();
  // u8g2.setCursor(0, 15);
  // u8g2.print("1.one");
  // u8g2.setCursor(0, 35);
  // u8g2.print("2.two");
  // u8g2.setCursor(0, 55);
  // u8g2.print("3.返回");
  // u8g2.sendBuffer();
  // switch (key) {
  //   case '1':
  //     SelectMode = true;
  //     state = true;
  //     break;
  //   case '2':
  //     SelectMode = false;
  //     state = true;
  //     break;
  //   case '3':
  //     show_menu();
  //     return;
  // }
  // if (state) {
  //   break;
  // }
  // }
  // int8_t counter = 0;
  //輸入位置
  while (1) {
    char key = keypad.getKey();

    u8g2.clearBuffer();
    u8g2.setCursor(0, 15);
    if (SelectMode) {
      u8g2.print("左右眼 INPUT");
    }
    // else if (counter == 0) {
    //   u8g2.print("右眼 INPUT");
    // } else {
    //   u8g2.print("左眼 INPUT");
    // }

    // u8g2.sendBuffer();

    if (key != '*' && key != '#' && key != 0) {

      stringThree += key;

      Serial.println(stringThree);
    }
    u8g2.setCursor(15, 0);
    u8g2.print("plus: ");
    u8g2.setCursor(15, 35);
    u8g2.print(stringThree);
    u8g2.sendBuffer();
    if (stringThree.length() == 4 || key == '#') {
      // if (SelectMode) {
      R = stringThree.toInt();
      L = stringThree.toInt();
      Serial.print("R=");
      Serial.println(R);
      Serial.print("L=");
      Serial.println(L);
      delay(1000);
      // show_menu();
      stringThree = "";
      break;
      // } else if (counter == 0) {
      //   R = stringThree.toInt();
      //   stringThree = "";
      //   counter++;
      // } else if (counter == 1) {
      //   L = stringThree.toInt();
      //   Serial.print("R=");
      //   Serial.println(R);
      //   Serial.print("L=");
      //   Serial.println(L);
      //   delay(1000);
      //   counter = 0;
      //   break;
      // }
    }
  }
  stringThree = "";
  //輸入速度
  while (1) {
    char key = keypad.getKey();
    u8g2.clearBuffer();
    u8g2.setCursor(0, 15);
    // if (SelectMode) {
    u8g2.print("左右眼速度");
    // }
    // else if (counter == 0) {
    //   u8g2.print("右眼速度");
    // } else {
    //   u8g2.print("左眼速度");
    // }

    // u8g2.sendBuffer();

    if (key != '*' && key != '#' && key != 0) {

      stringThree += key;

      Serial.println(stringThree);
    }
    u8g2.setCursor(15, 0);
    u8g2.print("speed= ");
    u8g2.setCursor(15, 35);
    u8g2.print(stringThree);
    u8g2.sendBuffer();
    if (stringThree.length() == 4 || key == '#') {
      // if (SelectMode) {
      R_speed = stringThree.toInt();
      L_speed = stringThree.toInt();
      Serial.print("R_speed=");
      Serial.println(R_speed);
      Serial.print("L_speed=");
      Serial.println(L_speed);
      delay(1000);
      break;

      // }

      //  else if (counter == 0) {
      //   R_speed = stringThree.toInt();
      //   stringThree = "";
      //   counter++;
      // } else if (counter == 1) {
      //   L_speed = stringThree.toInt();
      //   Serial.print("R_speed=");
      //   Serial.println(R);
      //   Serial.print("L_speed=");
      //   Serial.println(L_speed);
      //   delay(1000);
      //   counter = 0;
      //   show_menu();
      //   return;
      // }
    }
  }
  play(R_speed, R);
}
void play(int speed, int data_pos) {
  // void espnow_control(int Motor_number,int mode,int pulse,bool dir,int speed,bool res) {
  espnow_control(3, 0, data_pos, 0, speed, 0);
  u8g2.clearBuffer();
  u8g2.setCursor(0, 15);
  u8g2.print("執行中...");
  u8g2.setCursor(0, 35);
  u8g2.print("按＃停止");
  u8g2.sendBuffer();
  while (1) {
    char key = keypad.getKey();
    if (key == '#') {
      espnow_control(3, 1, data_pos, 0, speed, 0);
      show_menu();
      break;
    }
  }
}
