#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <SoftwareSerial.h>  
#include <DHT.h>

#define START_1 0x42  
#define START_2 0x4d  
#define DATA_LENGTH_H        0  
#define DATA_LENGTH_L        1  
#define PM1_0_ATMOSPHERE_H   8  
#define PM1_0_ATMOSPHERE_L   9  
#define PM2_5_ATMOSPHERE_H   10  
#define PM2_5_ATMOSPHERE_L   11  
#define PM10_ATMOSPHERE_H    12  
#define PM10_ATMOSPHERE_L    13  
#define PM2_5_PARTICLE_H   16  
#define PM2_5_PARTICLE_L   17  
#define VERSION              26  
#define ERROR_CODE           27  
#define CHECKSUM             29  #define DHTTYPE DHT22 

#define DHTTYPE DHT22

int btnPin = 8;       // 스위치(SW) 핀 설정
int ledPin = 10;
int dhtPin = 4;
int pmsPin = 7;       // LED 핀 설정
int mode = 1;
 
int reading;          // SW 상태
int previous = LOW;   // SW 이전 상태
 
long time = 0;        // LED가 ON/OFF 토글된 마지막 시간
long debounce = 100;  // Debounce 타임 설정

SoftwareSerial mySerial(7,6); // Arudino Uno port RX, TX  
LiquidCrystal_I2C lcd(0x27, 16, 2);
DHT dht(dhtPin, DHTTYPE);

byte bytCount1 = 0;  
byte bytCount2 = 0;  
unsigned char chrRecv;  
unsigned char chrData[30];  
int PM01;  
int PM25;  
int PM10;  
      
unsigned int GetPM_Data(unsigned char chrSrc[], byte bytHigh, byte bytLow)  
{  
   return (chrSrc[bytHigh] << 8) + chrSrc[bytLow];  
}  
      
void setup(){  
   Serial.begin(9600);  
   Serial.println("Serial test!! ");  
   mySerial.begin(9600);  

   pinMode(btnPin, INPUT_PULLUP);

   lcd.begin();
	lcd.backlight();

   dht.begin();
}  
      
void loop(){

  reading = digitalRead(btnPin);

  if (reading == LOW && previous == HIGH && millis() - time > debounce) {
    if (mode == 1){
       mode = 0;
      Serial.println("0");
    }  
     
    else{
        mode = 1;
        Serial.println("1!!");
    }        
    time = millis();
  }
  complexScouter(mode);
  previous = reading;
}

void complexScouter(int mode)
{
  //delay(2000);
  static unsigned long count = 0;
      if(count++ < 100000) return;
      count = 0;

  if (mode == 0)
  {
    lcd.clear();
    lcd.setCursor(0,0);
    if (mySerial.available())   {  
      for(int i = 0; i < 32; i++)     {  
         chrRecv = mySerial.read();  
         if (chrRecv == START_2 ) {   
            bytCount1 = 2;  
               break;  
         }  
      }   
      if (bytCount1 == 2){  
         bytCount1 = 0;  
         for(int i = 0; i < 30; i++){  
            chrData[i] = mySerial.read();  
         }   
      
         if ( (unsigned int) chrData[ERROR_CODE] == 0 ) {  
            PM01  = GetPM_Data(chrData, PM1_0_ATMOSPHERE_H, PM1_0_ATMOSPHERE_L);  
            PM25  = GetPM_Data(chrData, PM2_5_ATMOSPHERE_H, PM2_5_ATMOSPHERE_L);  
            PM10  = GetPM_Data(chrData, PM10_ATMOSPHERE_H, PM10_ATMOSPHERE_L);  
            lcd.print("PM1.0=");  
            lcd.print(PM01);  
            lcd.setCursor(0,1);
            lcd.print("PM2.5=");  
            lcd.print(PM25);  
            lcd.print(" PM10=");  
            lcd.println(PM10);  
         }else{  
            Serial.println("PMS7003  ERROR");  
         }  
      }   
   }  
   else{  
      Serial.println("PMS7003 NOT available");  
    }  
  } else if (mode == 1){
    lcd.clear();

    float h = dht.readHumidity();
    float t = dht.readTemperature();

    if (isnan(h) || isnan(t)) {
      Serial.println(F("Failed to read from DHT sensor!"));
      lcd.setCursor(0,0);
      lcd.print("Failed to read from DHT sensor!");
      return;
    }

    lcd.setCursor(0,0); // Sets the location at which subsequent text written to the LCD will be displayed
    lcd.print("Temp.: "); // Prints string "Temp." on the LCD
    lcd.print(t); // Prints the temperature value from the sensor
    lcd.print(" C");     
    lcd.setCursor(0,1);
    lcd.print("Humi.: ");
    lcd.print(h);
    lcd.print(" %"); 
   }
}
