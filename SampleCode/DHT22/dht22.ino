#include <DHT.h>

#define dhtPin 8
#define DHTTYPE DHT22

DHT dht(dhtPin, DHTTYPE);
float humi;
float temp;

void setup()
{
    Serial.begin(9600);
    dht.begin();
}

void loop()
{
    delay(3000);
    humi = dht.readHumidity();
    temp = dht.readTemperature();

   if (isnan(humi) || isnan(temp)) {
      Serial.println(F("Failed to read from DHT sensor!"));
      return;
   }
   Serial.print("Temp : ");
   Serial.print(temp);
   Serial.print(" C , humi : ");
   Serial.print(humi);
   Serial.println(" %");
}
