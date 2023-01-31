#define VER "AquaLED_040420"
#include <Wire.h>
#include <OneWire.h>
#include <EEPROM.h>
#include <microDS18B20.h>
#include <SoftwareSerial.h> //библиотека последовательной связи

#include <Adafruit_PWMServoDriver.h>
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(0x42);
OneWire ds(12); // Объект OneWire
#define DS3231_ADDRESS 0x68
//MicroDS18B20<12> sensor;
SoftwareSerial HM10(0, 1);

byte addr[8];
float Temp_val; 
//int fanSpeed;
byte flagDallRead;
byte data[12]; //для температуры

//#define CLK 2
//#define DT 3
//#define SW 4
//BT переменные для синизуба
#define PARSE_AMOUNT 13         // число значений в массиве, который хотим получить
int intData[PARSE_AMOUNT];     // массив численных значений после парсинга
boolean recievedFlag;
boolean getStarted;
boolean PWMSaveflag;
byte index;
String string_convert = "";
//BT переменные для синизуба
boolean PWMflag;



///////////////////////////////////конфигурация подключения нагрузки
#define cool_pin 6
//#define ch_1_pin 7
//#define ch_2_pin 8
//#define ch_3_pin 9
//#define ch_4_pin 10
//#define co2_pin 11


/////// включение реле по плюсу
//#define ON HIGH
//#define OFF LOW

/////// включение реле по минусу
/*#define ON LOW
#define OFF HIGH*/
///////////////////////////////////конфигурация подключения нагрузки


int8_t h, m, s;
int8_t hr;

//*******RGB*******///

int rgb_motion [5][12]; //5- количество таймеров
long Day_time;

#define PWM_LR_MIN 0 //Если необходим ток покоя на LED - изменить эту константу 
// #define PWM_LR_MAX 255//Если необходимо ограничить максимальную яркость - уменьшить значение 
#define PWM_LR_PIN A2 //Пин порта, где будет ШИМ LR

#define PWM_LG_MIN 0 //Если необходим ток покоя на LED - изменить эту константу 
// #define PWM_LG_MAX 255//Если необходимо ограничить максимальную яркость - уменьшить значение 
#define PWM_LG_PIN A0 //Пин порта, где будет ШИМ LG 

#define PWM_LB_MIN 0 //Если необходим ток покоя на LED - изменить эту константу 
// #define PWM_LB_MAX 255 //Если необходимо ограничить максимальную яркость - уменьшить значение 
#define PWM_LB_PIN A1 //Пин порта, где будет ШИМ LB 

#define PWM_LW_MIN 0 //Если необходим ток покоя на LED - изменить эту константу 
// #define PWM_LW_MAX 255//Если необходимо ограничить максимальную яркость - уменьшить значение 
#define PWM_LW_PIN A4 //Пин порта, где будет ШИМ LW

#define PWM_LC_MIN 0 //Если необходим ток покоя на LED - изменить эту константу 
// #define PWM_LC_MAX 255//Если необходимо ограничить максимальную яркость - уменьшить значение 
#define PWM_LC_PIN A3 //Пин порта, где будет ШИМ LC


#define mn_ul 60UL //Дополнительные константы для удобства 
#define hr_ul 3600UL //Отражают соответствующие количества секунд 
#define d_ul 86400UL 

long sunrise_start;// = 11*hr+2*mn; //Начало восхода в 9 - 45 
long sunrise_duration;// = 30*mn_ul; //Длительность восхода 30 минут 
long sunset_start;// = 20*hr+07*mn; //начало заката в 21-15 
long sunset_duration;//  = 30*mn_ul; //Длительность заката 30 минут 
 
long  LR; 
long  LG; 
long LB;
long LW; 
long LC;
long moon;

byte pwm_moon =10;

//*******RGB*******///
float delta = 3;

int8_t x_temp = 20;

double counter = 0;


void setup(void)
{
  Serial.begin(9600);
  pinMode(3, INPUT_PULLUP);
  HM10.begin(9600);
//  pinMode(13, OUTPUT);
//  digitalWrite(13, LOW);
 // --------------------- СБРОС НАСТРОЕК ---------------------
  if (!digitalRead(3)) {          // если замкнута ножка на D3 на GND

   EEPROM.write(35,40);
   EEPROM.write(36,2);
  ///////rgb
  /////////////////rgb motion
   EEPROM.write(391,1);
   EEPROM.write(392,1);
   EEPROM.write(393,0);
   EEPROM.write(394,0);
   EEPROM.write(395,0); 
   
   EEPROM.write(400,11); 
   EEPROM.write(401,30); 
   EEPROM.write(402,60); 
   EEPROM.write(403,19); 
   EEPROM.write(404,00); 
   EEPROM.write(405,90); 
   EEPROM.write(406,75);
   EEPROM.write(407,75);
   EEPROM.write(408,75);
   EEPROM.write(409,75);
   EEPROM.write(410,75);
  
 /////////////////rgb motion
 
   ///moon motion

   EEPROM.write(420,21); 
   EEPROM.write(421,00); 
   EEPROM.write(422,60); 
   EEPROM.write(423,23); 
   EEPROM.write(424,15); 
   EEPROM.write(425,45); 
   EEPROM.write(426,0);
   EEPROM.write(427,0);
   EEPROM.write(428,75);
   EEPROM.write(429,0);
   EEPROM.write(430,0);
   ///moon motion
  }
////////// --------------------- СБРОС НАСТРОЕК ---------------------

x_temp = EEPROM.read(35);
delta = EEPROM.read(36);
  for (byte i = 0; i < 5; i++) {
    rgb_motion[i][0]=EEPROM.read(i + 391);
    if (rgb_motion[i][0]>0) {
      for (byte z = 0; z < 11; z++) {
        rgb_motion[i][z+1]=EEPROM.read(i*20 + z + 400);
      }
    }
  }
 
//  pinMode(ch_1_pin, OUTPUT);
//  pinMode(ch_2_pin, OUTPUT);
//  pinMode(ch_3_pin, OUTPUT);
//  pinMode(ch_4_pin, OUTPUT);
  //pinMode(co2_pin, OUTPUT);
  pinMode(cool_pin, OUTPUT);

  
  pwm.begin();
  pwm.setPWMFreq(1600);  // This is the maximum PWM frequency
 
  // save I2C bitrate
  uint8_t twbrbackup = TWBR;
  // must be changed after calling Wire.begin() (inside pwm.begin())
  TWBR = 12; // upgrade to 400KHz!
/****
  enc1.setType(TYPE2);
  attachInterrupt(0, isrCLK, CHANGE);    // прерывание на 2 пине! CLK у энка
  attachInterrupt(1, isrDT, CHANGE); // прерывание на 3 пине! DT у энка
  Wire.begin();
****/
}
// функции с часами =======================================================================================================
byte decToBcd(byte val){
// Convert normal decimal numbers to binary coded decimal
return ( (val/10*16) + (val%10) );
}

byte bcdToDec(byte val) {
// Convert binary coded decimal to normal decimal numbers
return ( (val/16*10) + (val%16) );
}

void setHour(byte hour1){
Wire.beginTransmission(DS3231_ADDRESS);
Wire.write(0x02); //stop Oscillator
Wire.write(decToBcd(hour1));
Wire.endTransmission();
}

void setMinute(byte min1){

Wire.beginTransmission(DS3231_ADDRESS);
Wire.write(0x01); //stop Oscillator
Wire.write(decToBcd(min1));
Wire.endTransmission();
}

void READ_TIME_RTC(){
  Wire.beginTransmission(DS3231_ADDRESS);        //104 is DS3231 device address
  Wire.write(0x00);                                  //Start at register 0
  Wire.endTransmission();
  Wire.requestFrom(DS3231_ADDRESS, 7);           //Request seven bytes
      if(Wire.available()){ 
    s = Wire.read();                           //Get second
    m = Wire.read();                           //Get minute
    h   = Wire.read();                           //Get hour
   /* day     = Wire.read();
    date    = Wire.read();
    month   = Wire.read();                           //Get month
    year    = Wire.read();*/
       
    s = (((s & B11110000)>>4)*10 + (s & B00001111));   //Convert BCD to decimal
    m = (((m & B11110000)>>4)*10 + (m & B00001111));
    h   = (((h & B00110000)>>4)*10 + (h & B00001111));       //Convert BCD to decimal (assume 24 hour mode)
   /* day     = (day & B00000111); // 1-7
    date    = (((date & B00110000)>>4)*10 + (date & B00001111));         //Convert BCD to decimal  1-31
    month   = (((month & B00010000)>>4)*10 + (month & B00001111));       //msb7 is century overflow
    year    = (((year & B11110000)>>4)*10 + (year & B00001111));*/
  //  Serial.print(String(h)+":"+String(m)+":"+String(s));
  }
}
//
// ========================================================================================================================

void loop() // основной программный цикл
{
  
  READ_TIME_RTC();
  //dallRead();
  dallRead(flagDallRead * 1000);
  PWM_LED();
  parsing();
  btcommand();

}

