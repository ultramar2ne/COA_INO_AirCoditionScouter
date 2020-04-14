#include <PMS.h> 
#include <LiquidCrystal_I2C.h>
#include <SoftwareSerial.h>  
#include <DHT.h>
#include <MsTimer2.h>

// pin number
#define DHTTYPE DHT22  
#define btnPin 9
#define dhtPin 8
#define ledGreen  3
#define ledYellow 4
#define ledRed    5

int mode = 1;

int ledPin[] = {ledGreen,ledYellow,ledRed}; //ledpin number array -green, yellow, red

int reading;   //btn 상태
int previous = LOW;  //btn 이전 상태

long time = 0;
long debounce = 1000; 

SoftwareSerial mySerial(7 ,6);      // Arudino port RX, TX  for pms7003
LiquidCrystal_I2C lcd(0x27, 16, 2);
DHT dht(dhtPin, DHTTYPE);
PMS pms(mySerial);
PMS::DATA data;

byte bytCount1 = 0;  
byte bytCount2 = 0;  
unsigned char chrRecv;  
unsigned char chrData[30];  
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
   
   pinMode(btnPin, INPUT_PULLUP);   //initialize btn with pullup mode
   pinMode(ledPin[0],OUTPUT);
   pinMode(ledPin[1],OUTPUT);
   pinMode(ledPin[2],OUTPUT);
  
   pms.passiveMode();
   dht.begin();
   lcd.begin();
   lcd.backlight();

   infoUpdate();

   MsTimer2::set(10000,infoUpdate); // infoUpdate after every 10000ms
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

void ledStateListener()
{
   for(int i = 0 ; i < 3 ; i++)  //initialize ledstate
      digitalWrite(ledPin[i],LOW);

   if (PM25 <= 15  && PM10 <= 30)
   {
      digitalWrite(ledPin[0],HIGH);
   }else if (PM25 <= 35 && PM10 <= 80)
   {
      digitalWrite(ledPin[1],HIGH);
   }else
   {
      digitalWrite(ledPin[2],HIGH);
   }  
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
      lcd.print("PM2.5: ");  
      lcd.print(PM25);  
      lcd.setCursor(0,1);
      lcd.print("PM10 : ");  
      lcd.print(PM10);
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
   pms.wakeUp();
   pms.requestRead();
   if (pms.readUntil(data))
   {
      PM25 = data.PM_AE_UG_2_5;
      PM10 = data.PM_AE_UG_10_0;
      ledStateListener();
   }
   else
   {
      Serial.println("PMS7003 ERROR");
   }
   pms.sleep();

   //lcd update
   infoWrite(mode);
}
