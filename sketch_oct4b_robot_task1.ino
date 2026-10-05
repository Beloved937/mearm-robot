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
/*
左右遥感按键引脚
leftbuttonpin
rightbuttonpin
*/
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
void armjoycmd(char name,int topos){
int frompos;
Servo myservo;
int minpos,maxpos;
switch(name){
  case 's':
  myservo=shoulder;
  frompos=myservo.read();
  minpos=shouldermin;
  maxpos=shouldermax;
  break;
  case 'e':
  myservo=elbow;
  frompos=myservo.read();
  minpos=elbowmin;
  maxpos=elbowmax;
  break;
case 'b':
  myservo=base;
  frompos=myservo.read();
  minpos=basemin;
  maxpos=basemax;
  break;
case 'c':
  myservo=claw;
  frompos=myservo.read();
  minpos=clawmin;
  maxpos=clawmax;
  break;
default:
  break;
}
topos=constrain(topos,minpos,maxpos);
if(frompos<topos){
  myservo.write(frompos+1);
  delay(DSD);
}else if(frompos>topos){
  myservo.write(frompos-1);
  delay(DSD);
}
}
void armdatacmd(char q){
  if(q=='c'||q=='e'||q=='s'||q=='b'){
  int topos=Serial.parseInt();
  if(Serial.available()>0){
    Serial.read();
  }
  armcmd(q,topos,DSD);
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
  default:
  Serial.println("false");
  break;
  }
  }
}
void armcmd(char a,int topos,int DSD){
  int frompos;
  Servo myservo;
  int minpos,maxpos;
  switch(a){
    case 'c':
    frompos=claw.read();
    myservo=claw;
    minpos=clawmin;
    maxpos=clawmax;
    break;
    case 'e':
    frompos=elbow.read();
    myservo=elbow;
    minpos=elbowmin;
    maxpos=elbowmax;
    break;
    case 's':
    frompos=shoulder.read();
    myservo=shoulder;
    minpos=shouldermin;
    maxpos=shouldermax;
    break;
    case 'b':
    frompos=base.read();
    myservo=base;
    minpos=basemin;
    maxpos=basemax;
    break;
    default:
    break;
  }
  if(frompos>topos){
    for(int d=frompos;d>=topos&&d>=minpos;d--){
      myservo.write(d);
      delay(DSD);
    }
    }else if(frompos<topos){
      for(int d=frompos;d<=topos&&d<=maxpos;d++){
        myservo.write(d);
        delay(DSD);
      }
    }
}
int Gettoposleft(int analog){
  int a=90+map(analog,0,1024,0,180)-map(leftmiddata,0,1024,0,180);
   return a;
}
int Gettoposright(int analog){
  int a=90+map(analog,0,1024,0,180)-map(rightmiddata,0,1024,0,180);
   return a;
}