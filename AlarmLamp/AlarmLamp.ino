/*
 * 2018/05/05 オリハラ
 * パトランププロト ver.1.0
 * コードと配線が頭悪すぎる。
 */

 #include <Wire.h>

 #define PINNO 2   // 圧電スピーカを接続したピン番号

int pin[8];
int temp, j1, j2;
int pinLen = 8;
int readP = 2;

#define BEAT 300   // 音の長さを指定

void setup() {
  for(int i=0; i<pinLen; i++){
    pin[i] = i + 5; // setup内でやらないと途中でランプが止まる(原因不明)
    pinMode(pin[i], OUTPUT);
  }
  pinMode(readP, INPUT);
}

void loop() {
  LampOn();
}
void LampOn(){
   tone(PINNO,282,BEAT);
    for(int j = 0; j<pinLen; j++){
      j1 = j + 1;
      j2 = j + 2;
      if(j1 == pinLen) j1 = 1;
      if(j2 == pinLen) j2 = 0;
      digitalWrite(pin[j], HIGH);
      delay(20);
      digitalWrite(pin[j1], HIGH);
      delay(20);
      digitalWrite(pin[j2], HIGH);
      delay(20);
      temp = j - 1;
      if(temp == -1) temp = pinLen-1;
      digitalWrite(pin[temp], LOW);
      delay(20);
  }
}
void LampOff(){
  for(int k = 0; k<pinLen; k++){
      digitalWrite(pin[k], LOW);
      delay(20);
    }
}
