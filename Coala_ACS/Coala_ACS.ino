#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>

#define DHTPIN 2    
#define DHTTYPE DHT22 

DHT dht(DHTPIN, DHTTYPE);
LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {
  Serial.begin(9600);
  Serial.println(F("DHTxx test!"));

  lcd.begin();
  lcd.backlight();

  dht.begin();

}

void loop()
{
    delay(2000);
    lcd.clear();

    float h = dht.readHumidity();
    float t = dht.readTemperature();

    if (isnan(h) || isnan(t)) {
        Serial.println(F("Failed to read from DHT sensor!"));
        lcd.setCursor(0,1);
        lcd.print("Failed to read from DHT sensor!");
        return;
    }

    lcd.setCursor(2,1); // Sets the location at which subsequent text written to the LCD will be displayed
    lcd.print("Temp.: "); // Prints string "Temp." on the LCD
    lcd.print(t); // Prints the temperature value from the sensor
    lcd.print(" C");
    lcd.setCursor(2,2);
    lcd.print("Humi.: ");
    lcd.print(h);
    lcd.print(" %");
    
}

