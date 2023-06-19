//警告除非你知道你在做什麼，否則別改這頁的內容需要自定義參數請修改config.h
//C0:49:EF:BB:7E:58
#include "config.h"
#include <esp_now.h>
#include <WiFi.h>
#include <AccelStepper.h>
#include <Keypad.h>
#include <U8g2lib.h>
#include <Wire.h>
#include "stdint.h"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#define I2C_SDA 21
#define I2C_SCL 22
long currentPosition1;  //步進馬達當前位置
long currentPosition2;  //步進馬達當前位置
bool espnow_quit = 0;
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

//oled物件
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/U8X8_PIN_NONE);
// 步進馬達物件
AccelStepper stepper1(AccelStepper::DRIVER, STEP_PIN_1, DIR_PIN_1);
AccelStepper stepper2(AccelStepper::DRIVER, STEP_PIN_2, DIR_PIN_2);
//鍵盤物件
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, rows, cols);

//菜單狀態請勿修改//
bool show_menu_stest = true;
int8_t pages = 1;
int pulse = 0;
bool playStest=false;


// 傳輸資料的結構體
typedef struct struct_message {
  int Motor_number;  //0=同步 1＝1號馬達/ 2=二號馬達 /3 =執行主程式
  int mode;          //模式 0=位置模式 //1=增量控制
  int pulse;         //mode=0 指定移動的位置 //mode=1 移動的距離
  bool dir;          //方向 //mode=0 此方法無效 //mode=1 0=正轉/1=反轉 //此方法以機構指針方向定義非馬達方向
  int speed;         //速度
  bool res;          // 0＝正常狀態/1＝歸零
} struct_message;
// Create a struct_message called myData
struct_message myData;


void Task1_senddata(void * pvParameters ) {
  while(1){
    vTaskDelay(1);
    // Serial.println("Task1 run");
    if(playStest&&espnow_quit == 0){
      play(myData.speed, myData.pulse);
    }else if(playStest&&espnow_quit == 1){
      quit(); 
      playStest=false;
    }
    
  }
  
}
void Task2_senddata(void * pvParameters ) {
  while(1){
    int8_t key = get_key();
    if (key > 0) {
      OptionsUpdate(key);
  }
  }
}
TaskHandle_t Task1;
TaskHandle_t Task2;
void setup() {


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
  // // 設定 ESPNOW 對等裝置（Sender）
  // esp_now_peer_info_t peerInfo;
  // memcpy(peerInfo.peer_addr, senderMacAddress, sizeof(senderMacAddress));
  // peerInfo.channel = ESPNOW_CHANNEL;
  // peerInfo.encrypt = false;

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Once ESPNow is successfully Init, we will register for recv CB to
  // get recv packer info
  esp_now_register_recv_cb(OnDataRecv);




  // pinMode(STEP, OUTPUT);
  // pinMode(DIR, OUTPUT);
  pinMode(EN_1, OUTPUT);
  pinMode(EN_2, OUTPUT);
  digitalWrite(EN_1, LOW);
  digitalWrite(EN_2, LOW);
  stepper1.setMaxSpeed(500);      // 步進馬達最大速度
  stepper1.setAcceleration(500);  // 步進馬達加速度
  stepper2.setMaxSpeed(500);
  stepper2.setAcceleration(500);

  // 初始位置
  stepper1.setCurrentPosition(0);
  stepper2.setCurrentPosition(0);

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
  xTaskCreatePinnedToCore(
    Task1_senddata, /*任務實際對應的Function*/
    "Task1",        /*任務名稱*/
    10000,          /*堆疊空間*/
    NULL,           /*無輸入值*/
    0,                 /*優先序0*/
    &Task1,       /*對應的任務變數位址*/
    0);                /*指定在核心0執行 */
  xTaskCreatePinnedToCore(
    Task2_senddata, /*任務實際對應的Function*/
    "Task1",        /*任務名稱*/
    10000,          /*堆疊空間*/
    NULL,           /*無輸入值*/
    0,                 /*優先序0*/
    &Task2,       /*對應的任務變數位址*/
    1);     
  // run();
}
// callback function that will be executed when data is received
void OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len) {
  memcpy(&myData, incomingData, sizeof(myData));
  Serial.print("Bytes received: ");
  Serial.println(len);
  Serial.print("Char: ");
  Serial.println(myData.mode);
  Serial.print("Int: ");
  Serial.println(myData.pulse);
  Serial.print("Float: ");
  Serial.println(myData.dir);
  Serial.print("Bool: ");
  Serial.println(myData.speed);
  Serial.println();
  u8g2.clearBuffer();

  u8g2.setCursor(0, 35);
  u8g2.print("remote mode");

  u8g2.sendBuffer();

  if (myData.res == 1) {
    if (myData.Motor_number == 1) {
      stepper1.setCurrentPosition(0);  //設定0點位置
    } else if (myData.Motor_number == 2) {
      stepper2.setCurrentPosition(0);  //設定0點位置
    }
  }

  if (myData.Motor_number == 0) {  //同步運轉
    if (myData.mode == 0) {        //位址模式
      moveStepperToPosition(stepper1, myData.pulse);
      moveStepperToPosition(stepper2, myData.pulse);
      moto_run();
    } else if (myData.mode == 1) {  //增量模式
      if (myData.dir == 0) {        //正轉
        moveStepperToPosition(stepper1, stepper1.currentPosition() + myData.pulse);
        moveStepperToPosition(stepper2, stepper2.currentPosition() + myData.pulse);
        moto_run();
      } else {  //反轉
        moveStepperToPosition(stepper1, -myData.pulse - stepper1.currentPosition());
        moveStepperToPosition(stepper2, -myData.pulse - stepper2.currentPosition());
        moto_run();
      }
    }
  } else if (myData.Motor_number == 1) {  //一號馬達單獨控制
    if (myData.mode == 0) {               //位址模式
      moveStepperToPosition(stepper1, myData.pulse);
      // moveStepperToPosition(stepper2, myData.pulse);
      moto_run();
    } else if (myData.mode == 1) {  //增量模式
      if (myData.dir == 0) {        //正轉
        moveStepperToPosition(stepper1, stepper1.currentPosition() + myData.pulse);
        // moveStepperToPosition(stepper2, stepper2.currentPosition() + myData.pulse);
        moto_run();
      } else {  //反轉
        moveStepperToPosition(stepper1, -myData.pulse + stepper1.currentPosition());
        // moveStepperToPosition(stepper2, -myData.pulse - stepper2.currentPosition());
        moto_run();
      }
    }
  } else if (myData.Motor_number == 2) {  //一號馬達單獨控制
    if (myData.mode == 0) {               //位址模式
      // moveStepperToPosition(stepper1, myData.pulse);
      moveStepperToPosition(stepper2, myData.pulse);
      moto_run();
    } else if (myData.mode == 1) {  //增量模式
      if (myData.dir == 0) {        //正轉
        // moveStepperToPosition(stepper1, stepper1.currentPosition() + myData.pulse);
        moveStepperToPosition(stepper2, stepper2.currentPosition() + myData.pulse);
        moto_run();
      } else {  //反轉
        // moveStepperToPosition(stepper1, -myData.pulse - stepper1.currentPosition());
        moveStepperToPosition(stepper2, -myData.pulse + stepper2.currentPosition());
        moto_run();
      }
    }
  } else if (myData.Motor_number == 3) {  //執行主程式
    if (myData.mode != 1) {
      espnow_quit = 0;
      playStest=true;
      
    } else {
      espnow_quit = 1;
    }
  }
}

// 移動步進馬達到指定位置
void moveStepperToPosition(AccelStepper &stepper, long pos) {
  stepper.moveTo(pos);
}
void moto_run() {
  // 驅動兩顆馬達\

  while (1) {
    stepper1.run();
    stepper2.run();
    if (stepper1.distanceToGo() == 0 && stepper2.distanceToGo() == 0) {
      // 若兩顆馬達都到達目標位置，設定新的目標位置
      return;
    }
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
void loop() {
  vTaskDelay(1);
}
AccelStepper *EyeSelection() {
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

        return &stepper1;
      case 2:

        return &stepper2;
    }
  }
  return nullptr;
}

void ReturnToZero(int8_t options) {
  AccelStepper *Eye = EyeSelection();
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
        // currentPosition1 = stepper1.currentPosition();
        // currentPosition2 = stepper2.currentPosition();
        if (Eye != nullptr) {
          moveStepperToPosition(*Eye, Eye->currentPosition() + 20);  // 移動到位置1000和500

          moto_run();
        }
        break;
      case 2:
        if (Eye != nullptr) {
          moveStepperToPosition(*Eye, Eye->currentPosition() - 20);  // 移動到位置1000和500

          moto_run();
        }
        break;
      case 3:
        Eye->setCurrentPosition(0);  //設定0點位置
        Serial.print("1:");
        Serial.println(stepper1.currentPosition());
        Serial.print("2:");
        Serial.println(stepper1.currentPosition());
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
        // motor_state(800, true, 100, 0);
        // 移動到指定位置
        currentPosition1 = stepper1.currentPosition();
        currentPosition2 = stepper2.currentPosition();
        moveStepperToPosition(stepper1, currentPosition1 + 20);  // 移動到位置1000和500
        moveStepperToPosition(stepper2, currentPosition2 + 20);  // 移動到位置1000和500
        moto_run();

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
bool ExecutionState = 0;  //0=停止執行 1＝執行中
void play(int speed, int data_pos) {
  stepper1.setSpeed(speed);
  stepper2.setSpeed(speed);
  ExecutionState = 1;
  espnow_quit = 0;
  while (1) {  //正式執行

    currentPosition1 = stepper1.currentPosition();
    currentPosition2 = stepper2.currentPosition();
    u8g2.setCursor(15, 0);
    u8g2.print("plus= ");
    u8g2.setCursor(15, 35);
    u8g2.print(currentPosition1);
    u8g2.sendBuffer();
    moveStepperToPosition(stepper1, data_pos);
    moveStepperToPosition(stepper2, data_pos);
    moto_run();
    delay(1000);
    currentPosition1 = stepper1.currentPosition();
    currentPosition2 = stepper2.currentPosition();
    u8g2.clearBuffer();
    u8g2.setCursor(15, 0);
    u8g2.print("plus= ");
    u8g2.setCursor(15, 35);
    u8g2.print(currentPosition1);
    u8g2.sendBuffer();
    moveStepperToPosition(stepper1, 0);
    moveStepperToPosition(stepper2, 0);
    moto_run();
    delay(1000);
    quit();
    if (ExecutionState == 0) {
      break;
    }
    currentPosition1 = stepper1.currentPosition();
    currentPosition2 = stepper2.currentPosition();
    u8g2.clearBuffer();
    u8g2.setCursor(15, 0);
    u8g2.print("plus= ");
    u8g2.setCursor(15, 35);
    u8g2.print(currentPosition1);
    u8g2.sendBuffer();
    moveStepperToPosition(stepper1, -data_pos - currentPosition1);  // 移動到位置1000和500
    moveStepperToPosition(stepper2, -data_pos - currentPosition2);  // 移動到位置1000和500
    moto_run();
    delay(1000);
    quit();
    if (ExecutionState == 0) {
      break;
    }
    currentPosition1 = stepper1.currentPosition();
    currentPosition2 = stepper2.currentPosition();
    u8g2.clearBuffer();
    u8g2.setCursor(15, 0);
    u8g2.print("plus= ");
    u8g2.setCursor(15, 35);
    u8g2.print(currentPosition1);
    u8g2.sendBuffer();
    moveStepperToPosition(stepper1, 0);  // 移動到位置1000和500
    moveStepperToPosition(stepper2, 0);  // 移動到位置1000和500
    moto_run();
    delay(1000);
    quit();
    if (ExecutionState == 0) {
      break;
    }
  }
}

void quit() {
  char key = keypad.getKey();
  Serial.print("espnow_quit:");
  Serial.println(espnow_quit);
  if (key == '#' || espnow_quit == 1) {
    ExecutionState = 0;
    u8g2.clearBuffer();
    u8g2.setCursor(0, 35);
    u8g2.print("正在歸零...");

    u8g2.sendBuffer();
    moveStepperToPosition(stepper1, 0);  // 移動到位置1000和500
    moveStepperToPosition(stepper2, 0);  // 移動到位置1000和500
    moto_run();
    show_menu();
    
  }
}