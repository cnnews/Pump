#include <MsTimer2.h>
#include <TimeLib.h>

unsigned long light_t,pump_t,pMillis;
int flow_c,flow_cl;
float light,flow_l;

unsigned long previousMillis = 0;

void setup() {
  // put your setup code here, to run once:
  pinMode(13, OUTPUT);
  pinMode(10, OUTPUT);
  digitalWrite(10, HIGH);
  pinMode(9, OUTPUT);
  digitalWrite(9, HIGH);
  pinMode(2, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(2), flowc, FALLING);
  MsTimer2::set(500, flow_cal); // 500ms执行一次flow_cal
  MsTimer2::start();
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  //如果时间没有初始化，以2023年5月29日08点00分初始化时间
  if (timeStatus()== timeNotSet){
    setTime(1685347200);
  }

  unsigned long cMillis = millis();
if (cMillis - pMillis >= 100) {
  // save the last time you blinked the LED
  pMillis = cMillis;
  //光敏电阻滤波后输出
  light = analogRead(A0)*0.5+light*0.5;
  //计算水泵流量并滤波,Pulse frequency (Hz) = 6.6Q, Q is flow rate in L/min.Q=P/6.6*0.5s
  flow_l=flow_cl/1.65+flow_l*0.5;
}

  //无光线后延时600s/10min关闭输出
  if (light>50){
    digitalWrite(10, LOW);
    light_t=millis();
  } else {
    if (millis()-light_t>600000){
    digitalWrite(10, HIGH);
    }
  }

  //水泵运行条件,在8~18点，00分，20秒起动，延时6s后，如果流量<2L/min，或者运行时间达到15min停机
  if ((hour()>=8) && (hour()<=18) && (minute()==0) && (second()==20)){
    digitalWrite(9, LOW);
    pump_t=millis();
  }else{
    if (((millis()-pump_t>6000) && (flow_l<2)) || ((minute()==15) && (second()==20))){
    digitalWrite(9, HIGH);
    }
  }    
  //串口数据用于诊断↓
  unsigned long currentMillis = millis();
if (currentMillis - previousMillis >= 1000) {
    // save the last time you blinked the LED
    previousMillis = currentMillis;
  Serial.print(year());
  Serial.print("/");
  Serial.print(month());
  Serial.print("/");
  Serial.print(day());
  Serial.print("  ");
  Serial.print(hour());
  Serial.print(":");
  Serial.print(minute());
  Serial.print(":");
  Serial.print(second());
  Serial.print("-Light:");
  Serial.print(light);
  Serial.print("-FLow:");
  Serial.print(flow_l);
  Serial.print("-Count:");
  Serial.println(flow_c);
}
  //串口数据用于诊断↑
}
//脉冲计数
void flowc() {
  flow_c++;
}
//每500ms清空一次计数，用以计算流量
void flow_cal() {
  flow_cl=flow_c;
  flow_c=0;
}
