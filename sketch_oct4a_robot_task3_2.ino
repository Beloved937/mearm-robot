#include <Servo.h>
Servo base,claw,shoulder,elbow;
//调节各个舵机极限值
byte basemax=180;
byte basemin=0;
byte clawmax=130;
byte clawmin=28;
byte shouldermax=180;
byte shouldermin=0;
byte elbowmax=180;
byte elbowmin=0;
//各个舵机的引脚连接
byte basePin=9;
byte shoulderPin=8;
byte elbowPin=7;
byte clawPin=6;
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
//减速值
byte DSD=20;
//切换模式
const int MODE_JOY=0;//遥感模式
const int MODE_CMD=1;//电脑指令模式
int currentmod=MODE_JOY;

//左右遥感按键引脚
byte leftbuttonpin=5;
byte rightbuttonpin=4;
bool isrecoding = false;
byte recoding[200][4];
int recodingcount = 0;
long lasttime = 0;      // 录制开始时间
static long lasttime2 = 0; // 上次采样时间
//按钮1
int count1=0;
//左右摇杆中间数值
int leftmiddata=501;
int rightmiddata=485;
//绘制直线起始点
float x0,y0;
float x1,y1;
//机械臂数据
float L1=80.0;//大臂
float L2=80.0;//小臂
float d=55.0;//旋转中心到肩部高度
float elbowdata=65;
//移动到目标位置计数
int count2=0;
//绘制图形时的功能键
bool cancle=false;
bool pause=false;
void setup() {
   Serial.begin(9600);
  base.attach(basePin);
  shoulder.attach(shoulderPin);
  elbow.attach(elbowPin);
  claw.attach(clawPin);
  delay(100);
  base.write(90);
  claw.write(28);
  shoulder.write(90);
  elbow.write(180);
  delay(100);
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
if(isrecoding){
  long nowtime = millis();

  
  if(nowtime - lasttime > 15000){
    isrecoding = false;
    armcmd('b',90);
    armcmd('s',90);
    armcmd('e',180);
    armcmd('c',28);
    Serial.println("recoding is over");
  }
  else if(nowtime - lasttime2 > 80){
    if(recodingcount < 200){
      recoding[recodingcount][0] = base.read();
      recoding[recodingcount][1] = shoulder.read();
      recoding[recodingcount][2] = elbow.read();
      recoding[recodingcount][3] = claw.read();
      recodingcount++;
    }else{
      isrecoding = false;
      Serial.println("recoding full, stop");
    }
    lasttime2 = nowtime;
  }
}
}
void armjoycmd() {
  int leftRXdata=analogRead(A0);
  if(leftRXdata>leftmiddata&&leftRXdata-leftmiddata>50){
    armjoycmd('s',shoulder.read()+1);
  }else if(leftRXdata<leftmiddata&&leftmiddata-leftRXdata>50){
    armjoycmd('s',shoulder.read()-1);
  }
  int leftRYdata=analogRead(A1);
  if(leftRYdata>leftmiddata&&leftRYdata-leftmiddata>30){
    armjoycmd('e',elbow.read()+1);
  }else if(leftRYdata<leftmiddata&&leftmiddata-leftRYdata>30){
    armjoycmd('e',elbow.read()-1);
  }
  int rightRXdata=analogRead(A2);
  if(rightRXdata>rightmiddata&&rightRXdata-rightmiddata>30){
    armjoycmd('b',base.read()+1);
  }else if(rightRXdata<rightmiddata&&rightmiddata-rightRXdata>30){
    armjoycmd('b',base.read()-1);
  }
  int rightRYdata=analogRead(A3);
  if(rightRYdata>rightmiddata&&rightRYdata-rightmiddata>30){
    armjoycmd('c',claw.read()+1);
  }else if(rightRYdata<rightmiddata&&rightmiddata-rightRYdata>30){
    armjoycmd('c',claw.read()-1);
  }
}
void armjoycmd(char a,int topos){
  int frompos;
  Servo *myservo;
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
    myservo=&claw;
    minpos=clawmin;
    maxpos=clawmax;
     pLastTime = &lastTime_c;
    break;
    case 'e':
    frompos=elbow.read();
    myservo=&elbow;
    minpos=elbowmin;
    maxpos=elbowmax;
    pLastTime = &lastTime_e;
    break;
    case 's':
    frompos=shoulder.read();
    myservo=&shoulder;
    minpos=shouldermin;
    maxpos=shouldermax;
     pLastTime = &lastTime_s;
    break;
    case 'b':
    frompos=base.read();
    myservo=&base;
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
  if(frompos<topos){
  myservo->write(frompos+1);
}else if(frompos>topos){
  myservo->write(frompos-1);
  }
}
void armdatacmd(char q){
  if(q=='c'||q=='e'||q=='s'||q=='b'){
  int topos=Serial.parseInt();
  if(Serial.available()>0){
    Serial.read();
  }
  armcmd(q,topos);
  }
  else{
  switch(q){
  case 'O':
  armcmd('c',clawmin);
  break;
  case 'S':
 armcmd('c',clawmax);
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
  case '1':
  if(count1==0){
    armcmdA();
    count1++;
  }else if(count1==1){
    armcmdB();
    count1++;
  }else if(count1==2){
    armcmdC();
    count1=0;
  }
  break;
  case '2':
  recodingmode();
  break;
  case '3':
  if(recodingcount > 0){
    playrecoding();
  }
  break;
  case '4':
  armcmd('b',90);
  armcmd('s',90);
  armcmd('e',180);
  armcmd('c',28);
  break;
  case '5':
  if(count2==0){
  moveTo(0.0,30.0,75.0);
  count2++;
  }
  else if(count2==1){
  drawline(0.0,30.0,20.0,30.0,75.0);
  count2=0;
  }
  break;
  case '6':
  if(count2==0){
  moveTo(0.0,30.0,65.0);
  count2++;
  }
  else if(count2==1){
  drawline(0.0,30.0,80.0,30.0,65.0);
  drawline(80.0,30.0,0.0,85.0,65.0);
  count2=0;
  }
  break;
  default:
  while(Serial.available()>0){
    Serial.read();
  }
  break;
  }
  }
}
void armcmdA(void){
  int dataA[5][4]={
    {90,149,85,120},
    {90,90,180,120},
    {120,149,85,28},
    {120,90,180,28},
    {90,90,180,28}
  }; 
  armcmd('b',dataA[0][0]);
  armcmd('e',dataA[0][2]);
  armcmd('s',dataA[0][1]);
  armcmd('c',dataA[0][3]);
  for(int a=0;a<5;a++){
    armposrun(dataA[a][0],dataA[a][1],dataA[a][2],dataA[a][3]);
  }
}
void armcmdB(void){
  int dataB[5][4]={
    {120,112,108,120},
    {120,90,180,120},
    {30,112,108,28},
    {30,90,180,28},
    {90,90,180,28}
  };
  for(int a=0;a<5;a++){
    armposrun(dataB[a][0],dataB[a][1],dataB[a][2],dataB[a][3]);
  }
}
void armcmdC(void){
  int dataC[4][4]={
    {60,156,122,100},
    {60,90,180,100},
    {90,156,122,28},
    {90,90,180,28}
  };
  armcmd('b',dataC[0][0]);
  armcmd('e',dataC[0][2]);
  armcmd('s',dataC[0][1]);
  armcmd('c',dataC[0][3]);
  for(int a=1;a<4;a++){
    armposrun(dataC[a][0],dataC[a][1],dataC[a][2],dataC[a][3]);
  }
}
void armposrun(int base,int shoulder,int elbow,int claw){
 armcmd('b',base);
 armcmd('s',shoulder);
 armcmd('e',elbow);
 armcmd('c',claw);
}
void recodingmode(){
  if(!isrecoding){
    recodingcount = 0;
    isrecoding = true;
    lasttime = millis();
    lasttime2 = millis();
    Serial.println("recoding start");
  }else{
    isrecoding = false;
    Serial.println("recoding stop");
  }
}
void playrecoding(void){
  for(int a = 0; a < recodingcount; a++){
    base.write(recoding[a][0]);
    shoulder.write(recoding[a][1]);
    elbow.write(recoding[a][2]);
    claw.write(recoding[a][3]);
    delay(80);   // 和录制采样周期一致
  }
  recodingcount = 0;
}
void armcmd(char a,int topos){
  int frompos;
  Servo *myservo;
  int minpos,maxpos;
   switch(a){
    case 'c':
    frompos=claw.read();
    myservo=&claw;
    minpos=clawmin;
    maxpos=clawmax;
    break;
    case 'e':
    frompos=elbow.read();
    myservo=&elbow;
    minpos=elbowmin;
    maxpos=elbowmax;
    break;
    case 's':
    frompos=shoulder.read();
    myservo=&shoulder;
    minpos=shouldermin;
    maxpos=shouldermax;

    break;
    case 'b':
    frompos=base.read();
    myservo=&base;
    minpos=basemin;
    maxpos=basemax;
    break;
    default:
    return;
  }
  topos=constrain(topos,minpos,maxpos);
  if(frompos<topos){
    for(int d=frompos;d<=topos;d++){
      myservo->write(d);
      delay(DSD);
    }
  }else if(frompos>topos){
    for(int d=frompos;d>=topos;d--){
      myservo->write(d);
      delay(DSD);
    }
  }
}
void solveik(float x,float y,float z,float &baseangle,float &shoulderangle,float &elbowangle){
  baseangle = atan2(y, x) * 180 / PI;
  
  float cosShoulder = (L1*L1 + x*x + y*y + (z-d)*(z-d) - L2*L2) / (2*L1*sqrt(x*x + y*y + (z-d)*(z-d)));
  cosShoulder = constrain(cosShoulder, -1.0, 1.0); 
  shoulderangle = (atan2(z-d, sqrt(x*x + y*y)) + acos(cosShoulder)) * 180 / PI;
  float cosElbow = (x*x + y*y + (z-d)*(z-d) - L2*L2 - L1*L1) / (2*L2*L1);
  cosElbow = constrain(cosElbow, -1.0, 1.0);     
  elbowangle = acos(cosElbow) * 180 / PI;
}
void drawline(float x1,float y1,float x2,float y2,float zdraw){
  float theta1,theta2,theta3;
  int steps=200;
    for (int a = 0; a <= steps; a++) {
    if (Serial.available() > 0) {
      char cmd = Serial.read();
      
      if (cmd == '7') { 
        pause = !pause; 
      }
      else if (cmd == '8') { 
        cancle = true; 
        count2 = 0; 
      }
      
      while (Serial.available() > 0) {
        Serial.read();
      }
    }
    if(cancle==true){
      cancle=!cancle;
    return;
  }
  if(pause==true){
    while(true){
if(digitalRead(rightbuttonpin)==LOW){
  delay(50);
  if(digitalRead(rightbuttonpin)==LOW){
    pause=!pause;
    break;
    }
} 
if (Serial.available() > 0) {
        char cmd = Serial.read();
        if (cmd == '8') {
          cancle = true;
          while (Serial.available() > 0) Serial.read();
        }
      }
if(cancle==true){
  cancle=!cancle;
  return;
}
delay(10);
    }
  }
    float t=(float)a/(float)steps;
    float tox=x1+(x2-x1)*t;
    float toy=y1+(y2-y1)*t;
    solveik(tox,toy,zdraw,theta1,theta2,theta3);
  float physBase=theta1;
  float physShoulder=180.0-theta2;
  float physElbow=elbowdata+90+theta2-theta3;

  physBase=constrain(physBase,0,180);
  physShoulder=constrain(physShoulder,0,180);
  physElbow=constrain(physElbow,0,90+elbowdata);

  base.write(physBase);
  shoulder.write(physShoulder);
  elbow.write(physElbow);
  delay(15);
  }
}
void moveTo(float tx, float ty, float tz) {
  float theta1, theta2, theta3;
   solveik(tx, ty, tz, theta1, theta2, theta3);
    float targetBase = constrain(theta1, 0,180);
  float targetShoulder = constrain(180.0 - theta2, 0, 180);
  float targetElbow = constrain(elbowdata + 90.0 + theta2 - theta3, 0, 90+elbowdata);
  float currentBase = base.read();
  float currentShoulder = shoulder.read();
  float currentElbow = elbow.read();
for (int i = 0; i <= 80; i++) {
    float t = (float)i / 80.0;
    base.write(currentBase + (targetBase - currentBase) * t);
    shoulder.write(currentShoulder + (targetShoulder - currentShoulder) * t);
    elbow.write(currentElbow + (targetElbow - currentElbow) * t);
    delay(30); 
  }
}