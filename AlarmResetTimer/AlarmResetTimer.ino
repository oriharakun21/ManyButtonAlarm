/*
 * 2018/05/06 オリハラ
 * AlarmResetTimer ver.1.2
 * 時計と、アラームを止めるためのシステム
 * RTCライブラリの追加
 * 液晶i2cのライブラリがありません
 */
#include <Wire.h>
#include <Servo.h>
#include "RTClib.h"

// RTC
RTC_DS1307 RTC;
DateTime now;
char daysOfTheWeek[7][4] = {"Sun", "Mon", "Tues", "Wed", "Thu", "Fri", "Sat"};
boolean updateTime;//12時更新のためのフラグ 
// 液晶
#define PWR A1
#define GND A2
#define I2Cadr 0x3A
byte  VLCD = 60;  // 液晶の明るさ？（0 ～ 60?）
// 停止部分
const int out_startP = 2;
const int in_startP = 5;
const int out_buttonP = 3;
const int in_buttonP = 4;
// サーボ
 Servo myservo;
const int servoP = 6;
int a0; // サーボの可動域 0~179
// アラーム
boolean alarmF;
int setHour = 8;
int setMinute = 0;
const int setSecond = 0;
const int out_lampP = 7;
const int in_stopAlarm = 8;
const int in_alarmSetP = 12;

/* ---- セットアップ ---- */
void setup() {
  // 液晶
  pinMode(GND, OUTPUT);
  digitalWrite(GND, LOW);
  pinMode(PWR, OUTPUT);
  digitalWrite(PWR, HIGH);
  Wire.begin(); // i2cのマスタバスとして接続
  delay(40);
  firstSystem(); // おまじない
  delay(40);
  // 通信機器への出力
  pinMode(out_lampP, OUTPUT);
  // その他ピンの準備
  pinMode(out_startP, OUTPUT);
  pinMode(in_startP, INPUT);
  pinMode(out_buttonP, OUTPUT);
  pinMode(in_buttonP, INPUT);
  pinMode(in_stopAlarm, INPUT);
  pinMode(in_alarmSetP, INPUT);
  RTC.begin();
  myservo.attach(servoP);
   a0 = 110; // サーボの初期位置
   myservo.write(a0);
   delay(100);
   myservo.detach();
  alarmF = false;
  updateTime = true;
}

/* ---- ループ部分 ----- */
void loop() {
  if(alarmF){
    // アラーム
    alarmStopEnd();
    alarmStop();
  } else {
    // 時計
    now = RTC.now(); 
    outputScreenSecond();
    if(updateTime){
      lcd_setCursor(8, 1);
      lcd_printStr(" ORIHARA");
      outputScreenDay();
      updateTime=false;
      delay(60);
    }
    setTimes();
    if(digitalRead(in_stopAlarm) == HIGH) {
      alarmSet();
      delay(200);
    }
  }
  // 12時更新
  if(now.hour() == 0 && now.minute() == 0) updateTime = true;
}
// アラームの設定
void alarmSet(){
  delay(200);
  lcd_setCursor(0, 0);
  lcd_printStr("  + Alarm Set + ");
  lcd_setCursor(0, 1);
  lcd_printStr("     ");
  lcd_setCursor(5, 1);
  lcd_printNum(setHour);
  lcd_setCursor(7, 1);
  lcd_printStr(" ");
  lcd_setCursor(8, 1);
  lcd_printStr(":");
  lcd_setCursor(9, 1);
  lcd_printStr(" ");
  lcd_setCursor(10, 1);
  lcd_printNum(setMinute);
  lcd_setCursor(12, 1);
  lcd_printStr("     ");
  while(true){
      while(true){
      lcd_setCursor(5, 1);
      lcd_printStr("  ");
      delay(30);
      lcd_setCursor(5, 1);
      lcd_printNum(setHour);
      delay(160);
      if(digitalRead(in_alarmSetP) == HIGH) {
        setHour++;
        if(setHour >= 24) setHour = 0;
        delay(80);
      }
      if(digitalRead(in_stopAlarm) == HIGH) break; 
    }
    delay(200);
    while(true){
      lcd_setCursor(10, 1);
      lcd_printStr("  ");
      delay(30);
      lcd_setCursor(10, 1);
      lcd_printNum(setMinute);
      delay(160);
      if(digitalRead(in_alarmSetP) == HIGH) {
        setMinute=setMinute+5;
        if(setMinute >= 60) setMinute = 0;
        delay(80);
      }
      if(digitalRead(in_stopAlarm) == HIGH) break; 
    }
    updateTime=true;
  if(digitalRead(in_stopAlarm) == HIGH) break;
  }
}
// アラーム時間判定
void setTimes(){
  if(setHour == now.hour() && setMinute ==  now.minute() && setSecond == now.second()){
    myservo.attach(servoP);
    a0 = 30;
    myservo.write(a0);
    // delay(60);
    alarmF = true; // out_startPとセットのアラーム起動中フラグ
    digitalWrite(out_startP, HIGH);
    digitalWrite(out_lampP, HIGH);
    alarmMesserge();
  }
}
// アラームを止める部分の制御
void alarmStop(){
  if(digitalRead(in_startP) == HIGH){
    digitalWrite(out_buttonP, HIGH);
  } else {
    digitalWrite(out_buttonP, LOW);
  }
  // スイッチを押せる状態になったとき
  if(digitalRead(in_buttonP) == HIGH){
    a0 = 110;
    myservo.write(a0);
  }
}

void alarmStopEnd(){
  if(digitalRead(in_stopAlarm) == HIGH){
    alarmF = false; // out_startPとセットのアラーム起動中フラグ
    digitalWrite(out_startP, LOW);
    digitalWrite(out_lampP, LOW);
    updateTime=true;
    myservo.detach();
    delay(300);
  }
}
// アラーム作動中の表示
void alarmMesserge(){
  lcd_setCursor(0, 0);
  lcd_printStr(" ** Warning! ** ");
  lcd_setCursor(0, 1);
  lcd_printStr("     ");
  lcd_setCursor(5, 1);
  lcd_printNum(setHour);
  lcd_setCursor(7, 1);
  lcd_printStr(" : ");
  lcd_setCursor(10, 1);
  lcd_printNum(setMinute);
  lcd_setCursor(12, 1);
  lcd_printStr("     ");
}
// 画面への書き出し（位置と文字）
void outputScreenDay(){
  lcd_setCursor(0, 0);
  lcd_printNum(now.year());
  lcd_setCursor(4, 0);
  lcd_printStr("/");
  lcd_setCursor(5, 0);
  lcd_printNum(now.month());
  lcd_setCursor(7, 0);
  lcd_printStr("/");
  lcd_setCursor(8, 0);
  lcd_printNum(now.day());
  lcd_setCursor(10, 0);
  lcd_printStr(" ");
  lcd_setCursor(11, 0);
  lcd_printStr(daysOfTheWeek[now.dayOfTheWeek()]);
  lcd_setCursor(14, 0);
  lcd_printStr("  ");
}
void outputScreenSecond(){
  lcd_setCursor(0, 1);
  lcd_printNum(now.hour());
  lcd_setCursor(2, 1);
  lcd_printStr(":");
  lcd_setCursor(3, 1);
  lcd_printNum(now.minute());
  lcd_setCursor(5, 1);
  lcd_printStr(":");
  lcd_setCursor(6, 1);
  lcd_printNum(now.second());
}
// おまじない,初期化？
void firstSystem() {
  lcd_cmd(0x34);
  delay(5);
  lcd_cmd(0x34);
  delay(5);
  lcd_cmd(0x34);
  delay(40);
  Wire.beginTransmission(I2Cadr);
  Wire.write(0x00); // CO = 0,RS = 0
  Wire.write(0x35);
  Wire.write(0x41);
  Wire.write(0x80 | VLCD);
  Wire.write(0xC0 | VLCD);
  Wire.write(0x34);
  Wire.endTransmission();
  lcd_cmd(0x01);
  delay(400);
  lcd_cmd(0x0C);
  lcd_cmd(0x06);
}
byte decToBcd(byte val) {
  // 通常の10進数をバイナリコード10進数に変換する
  return ((val / 10 * 16) + (val % 10));
}
byte bcdToDec(byte val) {
  // バイナリコード10進数を通常の10進数に変換する
  return ((val / 16 * 10) + (val % 16));
}
// これらが何してるかが良く分からない
void lcd_cmd(unsigned char x) {
  Wire.beginTransmission(I2Cadr);
  Wire.write(0b00000000); // CO = 0,RS = 0
  Wire.write(x);
  Wire.endTransmission();
}
void lcd_data(unsigned char x) {
  Wire.beginTransmission(I2Cadr);
  Wire.write(0b01000000); // CO = 0, RS = 1
  Wire.write(x ^ 0x80);
  Wire.endTransmission();
}
// 文字の書き出し(テンプレ)
void lcd_printStr(const char *s) {
  Wire.beginTransmission(I2Cadr);
  while (*s) {
    if (*(s + 1)) {
      Wire.write(0b11000000); // CO = 1, RS = 1
      Wire.write(*s ^ 0x80);
    }
    else {
      Wire.write(0b01000000); // CO = 0, RS = 1
      Wire.write(*s ^ 0x80);
    }
    s++;
  }
  Wire.endTransmission();
}
// 数字の書き出し（20桁まで）
void lcd_printNum(int n) {
  // 全ての桁に対応しました。
  int temp[20];
  int count = 0;
  while(n != 0){
    temp[count] = n;
    n /= 10;
    while(temp[count] >= 10){
      temp[count]%= 10;
    }
    count++;
  }
  Wire.beginTransmission(I2Cadr);
  if(count <= 1) {
    for(int i=count; i<2; i++){
      Wire.write(0b11000000); // CO = 1, RS = 1
      Wire.write(48 ^ 0x80);
    }
  }
  for(int i=count-1;i>=0;i--){
    Wire.write(0b11000000); // CO = 1, RS = 1
    temp[i]=temp[i]+48;
    Wire.write(temp[i] ^ 0x80);
  }
  Wire.endTransmission();
}
// 表示位置
void lcd_setCursor(unsigned char x, unsigned char y) {
  lcd_cmd(0x80 | (y * 0x40 + x));
}
