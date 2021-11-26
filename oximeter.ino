#include <Wire.h>
#include "MAX30100_PulseOximeter.h"
#include <LiquidCrystal_I2C.h>
#define REPORTING_PERIOD_MS 1000  // 數據顯示間隔 (毫秒)
#define BEAT_LED 13  // 脈搏指示 LED 的腳位
PulseOximeter pox;
uint32_t tsLastReport = 0;
LiquidCrystal_I2C lcd(0x27, 16, 2);  // LCD I2C 位址和長寬




//蜂鳴器
const int buzzer = 8;

const int toneTable[7][5]={

    { 66, 131, 262, 523, 1046},  // C Do

    { 74, 147, 294, 587, 1175},  // D Re

    { 83, 165, 330, 659, 1318},  // E Mi

    { 88, 175, 349, 698, 1397},  // F Fa

    { 98, 196, 392, 784, 1568},  // G So

    {110, 220, 440, 880, 1760},  // A La

    {124, 247, 494, 988, 1976}   // B Si

};

char toneName[]="CDEFGAB";

char beeTone[]="GEEFDDCDEFGGGG";

int beeBeat[]={

    1,1,2, 1,1,2, 1,1,1,1,1,1,2,

    1,1,2, 1,1,2, 1,1,1,1,4,

    1,1,1,1,1,1,2, 1,1,1,1,1,1,2,

    1,1,2, 1,1,2, 1,1,1,1,4

};

int getTone(char symbol) {

    int toneNo = 0;

    for ( int ii=0; ii<7; ii++ ) {

        if ( toneName[ii]==symbol ) {

            toneNo = ii;

            break;

        }

    }

    return toneNo;

}

int resetPin = 4;  //設定reset腳位


void setup() {
  digitalWrite(resetPin, HIGH);
  pinMode(resetPin, OUTPUT);    //設定resetPin為輸出
  Serial.println("reset");//print reset to know the program has been reset and the setup function happened

  //pinMode(3,OUTPUT);//設定Pin3為輸出腳位 LDE燈
  pinMode(buzzer,OUTPUT);
  noTone(buzzer);


  
  pinMode(BEAT_LED, OUTPUT);
  digitalWrite(BEAT_LED, LOW);
  // 初始化 LCD
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.print("Arduino Oximeter");
  delay(1000);
  // 初始化 MAX30100, 若失敗就用無窮迴圈卡住
  if (!pox.begin()) {
    lcd.setCursor(0, 1);
    lcd.print("Failed to init!");
    while (true) {}
  }
  // 設定測到脈搏時要回呼的函式 (必須放在 setup 最後一行)
  pox.setOnBeatDetectedCallback(onBeatDetected);
}

void loop() {
  int ii, length, toneNo;
  int duration;
  length = sizeof(beeTone)-1;
  pox.update();  // 更新 MAX30100 讀數
  // 每隔指定的時間讀一次讀數, 並顯示在 LCD 上
  int a = 0;
  if (millis() - tsLastReport > REPORTING_PERIOD_MS) {
    float bpm = pox.getHeartRate();
    uint8_t spo2 = pox.getSpO2();
    lcd.clear();
    lcd.home();
    lcd.print("  Bpm : " + String(bpm));  // 顯示脈搏 bpm
    lcd.setCursor(0, 1);
    lcd.print("  SpO2: " + String(spo2) + "%");  // 顯示血氧濃度
    tsLastReport = millis();
    if ((int(spo2) < 97) && (int(spo2) != 0)){
      a = 1;
      }
  }
  if ( a == 1 ){
    digitalWrite(BEAT_LED, HIGH);
    for ( ii=0; ii<length; ii++ ) {
      toneNo = getTone(beeTone[00]);
      duration = beeBeat[00]*333;
      tone(buzzer,toneTable[toneNo][3]);
      delay(duration);
      noTone(buzzer);
      delay(1000);
      }
      digitalWrite(resetPin, LOW);
      }

}
void onBeatDetected() {
  // 點亮 LED 50 毫秒代表一次脈搏
  pinMode(BEAT_LED, OUTPUT);
  digitalWrite(BEAT_LED, HIGH);
  delay(50);
  digitalWrite(BEAT_LED, LOW);
}