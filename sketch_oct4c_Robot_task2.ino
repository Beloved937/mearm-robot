#include <Servo.h>
Servo base,claw,shoulder,elbow;
//调节各个舵机极限值
int basemax;
int basemin;
int clawmax;
int clawmin;
int shouldermax;
int shouldermin;
int elbowmax;
int elbowmin;
//各个舵机的引脚连接
int basePin;
int shoulderPin;
int elbowPin;
int clawPin;
//遥感对应引脚
/*
right:
VRX：A2
VRY:A3
left:
VRX:A0
VRY:A1
*/
//遥感中间值
/*
leftmiddata 501
rightmiddata 485 
*/
int leftmiddata;
int rightmiddata;
//减速值
int DSD=20;
//切换模式
const int MODE_JOY=0;//遥感模式
const int MODE_CMD=1;//电脑指令模式
int currentmod=MODE_JOY;

//左右遥感按键引脚
int leftbuttonpin;
int rightbuttonpin;

void setup() {
   Serial.begin(9600);
  base.attach(basePin);
  shoulder.attach(shoulderPin);
  elbow.attach(elbowPin);
  claw.attach(clawPin);
  base.write(90);
  claw.write(90);
  shoulder.write(90);
  elbow.write(90);
  pinMode(rightbuttonpin, INPUT_PULLUP);
  pinMode(leftbuttonpin, INPUT_PULLUP);
}

void loop() {
  static int keylast1=HIGH;
  int keynow1=digitalRead(leftbuttonpin);
if(keylast1==HIGH&&keynow1==LOW){
  delay(50);
  keynow1=digitalRead(leftbuttonpin);
  if(keynow1==LOW){
    currentmod=MODE_JOY;
    Serial.println("Int JOY MOD");
  }
}
keylast1=keynow1;
 static int keylast2=HIGH;
  int keynow2=digitalRead(rightbuttonpin);
if(keynow2==LOW&&keylast2==HIGH){
  delay(50);
  keynow2=digitalRead(rightbuttonpin);
  if(keynow2==LOW){
    currentmod=MODE_CMD;
    Serial.println("Int CMD MOD");
    }
}
keylast2=keynow2;
  switch(currentmod){
    case MODE_JOY:
    armjoycmd();
    break;
    case MODE_CMD:
    for(int a=0;a<3;a++){
    if(Serial.available()>0){
      char lable=Serial.read();
      armdatacmd(lable);
    }
  }
  break;
}
}
//遥感入口
void armjoycmd(){
  int leftRXtopos=Gettoposleft(analogRead(A0));//shoulder
   armjoycmd('s',leftRXtopos);
  int leftRYtopos=Gettoposleft(analogRead(A1));//elbow
   armjoycmd('e',leftRYtopos);
  int rightRXtopos=Gettoposright(analogRead(A2));//base
   armjoycmd('b',rightRXtopos);
  int rightRYtopos=Gettoposright(analogRead(A3));//claw
   armjoycmd('c',rightRYtopos);
}
//遥感所用函数,指令所用函数
void armjoycmd(char a,int topos){
  int frompos;
  Servo myservo;
  int minpos,maxpos;
  static unsigned long lastTime_s = 0;
  static unsigned long lastTime_e = 0;
  static unsigned long lastTime_b = 0;
  static unsigned long lastTime_c = 0;
  unsigned long nowTime = millis();
  unsigned long *pLastTime;
  switch(a){
    case 'c':
    frompos=claw.read();
    myservo=claw;
    minpos=clawmin;
    maxpos=clawmax;
     pLastTime = &lastTime_c;
    break;
    case 'e':
    frompos=elbow.read();
    myservo=elbow;
    minpos=elbowmin;
    maxpos=elbowmax;
    pLastTime = &lastTime_e;
    break;
    case 's':
    frompos=shoulder.read();
    myservo=shoulder;
    minpos=shouldermin;
    maxpos=shouldermax;
     pLastTime = &lastTime_s;
    break;
    case 'b':
    frompos=base.read();
    myservo=base;
    minpos=basemin;
    maxpos=basemax;
    pLastTime = &lastTime_b;
    break;
    default:
    return;
  }
  if(nowTime-*pLastTime<DSD){
    return;
  }
  *pLastTime=nowTime;
  topos = constrain(topos,minpos,maxpos);
  if(abs(topos-frompos)<2){
    return;
  }//防止抖动
  if(frompos<topos){
  myservo.write(frompos+1);
}else if(frompos>topos){
  myservo.write(frompos-1);
  }
}
//指令解析
void armdatacmd(char q){
  if(q=='c'||q=='e'||q=='s'||q=='b'){
  int topos=Serial.parseInt();
  if(Serial.available()>0){
    Serial.read();
  }
  armjoycmd(q,topos);
  }
  else{
  switch(q){
    case 'U':
    Serial.println(" ");
    Serial.print("claw pos:");
    Serial.print(claw.read());
    Serial.print(" ");
    Serial.print("The DSD of vaule:");
    Serial.println(DSD);
    Serial.print("elbow pos:");
    Serial.print(elbow.read());
    Serial.print(" ");
    Serial.print("The DSD of vaule:");
    Serial.println(DSD);
    Serial.print("shoulder pos:");
    Serial.print(shoulder.read());
    Serial.print(" ");
    Serial.print("The DSD of vaule:");
    Serial.println(DSD);
    Serial.print("base pos:");
    Serial.print(base.read());
    Serial.print(" ");
    Serial.print("The DSD of vaule:");
    Serial.println(DSD);
    Serial.println(" ");
    break;
    case 'l':
  claw.write(90);
  elbow.write(90);
  shoulder.write(90);
  base.write(90);
  break;
  case 'O':
  claw.write(clawmax);
  break;
  case 'S':
  claw.write(clawmin);
  break;
  case 'H':
  DSD=DSD-2;
  break;
  case 'L':
  DSD=DSD+2;
  break;
  case 'A':
  armcmdA();
  break;
  case 'B':
  armcmdB();
  break;
  case 'C':
  armcmdC();
  break;
  default:
  Serial.println("false");
  break;
  }
  }
}
//遥感模式下得到topos
int Gettoposleft(int analog){
  if(abs(analog-leftmiddata)<8){
    return 90;
  }
  int a=90+map(analog,0,1024,0,180)-map(leftmiddata,0,1024,0,180);
   return a;
}
int Gettoposright(int analog){
  if(abs(analog-rightmiddata)<8){
    return 90;
  }
  int a=90+map(analog,0,1024,0,180)-map(rightmiddata,0,1024,0,180);
   return a;
}
//实现物体A的抓取
void armcmdA(void){
  int dataA[6][4];
  for(int a=0;a<6;a++){
    armposrun(dataA[a][0],dataA[a][1],dataA[a][2],dataA[a][3]);
  }
}
//实现物体A的抓取
void armcmdB(void){
  int dataB[6][4];
  for(int a=0;a<6;a++){
    armposrun(dataB[a][0],dataB[a][1],dataB[a][2],dataB[a][3]);
  }
}//实现物体B的抓取
void armcmdC(void){
  int dataC[6][4];
  for(int a=0;a<6;a++){
    armposrun(dataC[a][0],dataC[a][1],dataC[a][2],dataC[a][3]);
  }
}//实现物体C的抓取
//抓取所用函数入口
void armposrun(int base,int shoulder,int elbow,int claw){
armjoycmd('b',base);
delay(DSD);
armjoycmd('s',shoulder);
delay(DSD);
armjoycmd('e',elbow);
delay(DSD);
armjoycmd('c',claw);
delay(DSD);
}