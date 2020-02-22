#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <SoftwareSerial.h>  
#include <DHT.h>
#include <MsTimer2.h>

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
#define CHECKSUM             29 

// pin number
#define DHTTYPE DHT22
#define btnPin 9
#define dhtPin 8
#define pmsPin 7

int mode = 1;

int ledPin[] = {0,1,2}; //ledpin number array -green, yellow, red

int reading;   //btn 상태
int previous = LOW;  //btn 이전 상태

long time = 0;
long debounce = 1000; 

SoftwareSerial mySerial(7 ,6); // Arudino Uno port RX, TX  for pms7003
LiquidCrystal_I2C lcd(0x27, 16, 2);
DHT dht(dhtPin, DHTTYPE);

byte bytCount1 = 0;  
byte bytCount2 = 0;  
unsigned char chrRecv;  
unsigned char chrData[30];  
int PM01;  
int PM25;  
int PM10;  

float humi;
float temp;
      
unsigned int GetPM_Data(unsigned char chrSrc[], byte bytHigh, byte bytLow)  
{  
   return (chrSrc[bytHigh] << 8) + chrSrc[bytLow];  
}  
      
void setup()
{

   Serial.begin(115200);
   mySerial.begin(9600);
   
   pinMode(btnPin, INPUT_PULLUP);
   lcd.begin();
   lcd.backlight();
   dht.begin();

   infoUpdate();

   MsTimer2::set(10000,infoUpdate);
   MsTimer2::start();

}

void loop() 
{
   reading = digitalRead(btnPin);

   if (reading == LOW && previous == HIGH && millis() - time > debounce)
   {
      if (mode == 1)
      {
         mode = 0;
         Serial.println("mode 0");
      } else{

         mode = 1;
         Serial.println("mode 1");
      }
      time = millis();
      infoWrite(mode);
   }
   previous = reading;
}

void infoWrite (int mode)
{

   lcd.clear();

   if (mode == 0)    // dht info print
   {
      lcd.setCursor(0,0); 
      lcd.print("Temp.: "); 
      lcd.print(temp); 
      lcd.print(" C");     
      lcd.setCursor(0,1);
      lcd.print("Humi.: ");
      lcd.print(humi);
      lcd.print(" %"); 
   } else if (mode == 1)   //pms info print
   {
      lcd.setCursor(0,0);
      lcd.print("PM1.0=");  
      lcd.print(PM01);  
      lcd.setCursor(0,1);
      lcd.print("PM2.5=");  
      lcd.print(PM25);  
      lcd.setCursor(8,1);
      lcd.print(" PM10=");  
      lcd.println(PM10);
   }
   
}

void infoUpdate () 
{
   // dht update
   humi = dht.readHumidity();
   temp = dht.readTemperature();

   if (isnan(humi) || isnan(temp)) {
      Serial.println(F("Failed to read from DHT sensor!"));
      lcd.setCursor(0,0);
      lcd.print("Failed to read from DHT sensor!");

      infoUpdate();
      return;
   }

   // pms update
   if (mySerial.available())
   {
      for (int i = 0; i < 32; i++)
      {
         chrRecv = mySerial.read();
         if (chrRecv == START_2)
         {
            bytCount1 = 2;
            break;
         }
      }

      if (bytCount1 == 2) 
      {
         bytCount1 = 0;
         for (int i = 0; i < 30; i++)
         {
            chrData[i] = mySerial.read();
         }

         if ((unsigned int) chrData[ERROR_CODE] == 0)
         {
            PM01  = GetPM_Data(chrData, PM1_0_ATMOSPHERE_H, PM1_0_ATMOSPHERE_L);  
            PM25  = GetPM_Data(chrData, PM2_5_ATMOSPHERE_H, PM2_5_ATMOSPHERE_L);  
            PM10  = GetPM_Data(chrData, PM10_ATMOSPHERE_H, PM10_ATMOSPHERE_L); 
         } else {
            Serial.println("PMS7003 ERROR");
            infoUpdate();
         }
      }
   } else
   {
      Serial.println("PMS7003 NOT available");  
      infoUpdate();
   }
   

   //lcd update
   infoWrite(mode);

}
