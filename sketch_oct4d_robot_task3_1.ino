const int keyPin1;   //按键1 循环抓取
const int keyPin2;   //按键2 录制
const int keyPin3;   //按键3 播放
const int keyPin4;   //按键4 回中

//按键状态保存，消抖
int lastKey1 = HIGH;
int lastKey2 = HIGH;
int lastKey3 = HIGH;
int lastKey4 = HIGH;

void setup() {
  pinMode(keyPin1, INPUT_PULLUP);
  pinMode(keyPin2, INPUT_PULLUP);
  pinMode(keyPin3, INPUT_PULLUP);
  pinMode(keyPin4, INPUT_PULLUP);
}

void loop() {
  //按键1检测
  int now1 = digitalRead(keyPin1);
  if(lastKey1 == HIGH && now1 == LOW){
    delay(50); 
    now1 = digitalRead(keyPin1);
    if(now1 == LOW){
      Serial.write('1'); 
    }
  }
  lastKey1 = now1;

  //按键2检测
  int now2 = digitalRead(keyPin2);
  if(lastKey2 == HIGH && now2 == LOW){
    delay(50);
    now2 = digitalRead(keyPin2);
    if(now2 == LOW){
      Serial.write('2');
    }
  }
  lastKey2 = now2;

  //按键3检测
  int now3 = digitalRead(keyPin3);
  if(lastKey3 == HIGH && now3 == LOW){
    delay(50);
    now3 = digitalRead(keyPin3);
    if(now3 == LOW){
      Serial.write('3');
    }
  }
  lastKey3 = now3;

  //按键4检测
  int now4 = digitalRead(keyPin4);
  if(lastKey4 == HIGH && now4 == LOW){
    delay(50);
    now4 = digitalRead(keyPin4);
    if(now4 == LOW){
      Serial.write('4');
    }
  }
  lastKey4 = now4;
}
