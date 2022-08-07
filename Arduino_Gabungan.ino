#include "DHT.h" //Library DHT 

const int RELAY_PIN = 4;
float lux; //Test Relay

#include <Wire.h> //Library BH1750
#include <BH1750.h> //Library BH1750
BH1750 lightMeter; //BH1750

#include <OneWire.h> //DS18B20
#include <DallasTemperature.h> //DS18B20
#define ONE_WIRE_BUS 7 //DS18B20
OneWire oneWire(ONE_WIRE_BUS); //DS18B20
DallasTemperature sensors(&oneWire); //DS18B20

#include <LiquidCrystal.h> //Library LCD

#include <SoftwareSerial.h>

#define THRESHOLD 50 //Soil Batas dan trigger relay
const int AirValue = 600;   //Soil
const int WaterValue = 300; //Soil
int soilMoistureValue = 0; //Soil
int soilmoisturepercent = 0; //Soil

#define DHTPIN 3 //DHT 22
#define DHTTYPE DHT22 //DHT 22
DHT dht(DHTPIN, DHTTYPE); //DHT 22

const int rs = 8, en = 9, d4 = 10, d5 = 11, d6 = 12, d7 = 13; //LCD
LiquidCrystal lcd(rs, en, d4, d5, d6, d7); //LCD

SoftwareSerial espSerial(17, 16); //RX, TX

String str;

struct sensor_template {
  float sense_bh1750;
  float sense_dht22_temp;
  float sense_dht22_hum;
  float sense_soilmst;
  float sense_ds18b20;
  int sense_relay = 0;
  //  int serial_ack;
} sensor_data;

void setup() {
  Serial.begin(115200);
  espSerial.begin(9600);
  Wire.begin();
  lightMeter.begin();
  dht.begin();
  sensors.begin();
  delay(1000);
}

void loop()
{
  udara();
  ds();
  soil();
  cahaya();
  relay();
  str = String(sensor_data.sense_dht22_temp) + String("#") + String(sensor_data.sense_dht22_hum) + String("#") + String(sensor_data.sense_ds18b20) + String("#") + String(sensor_data.sense_soilmst) + String("#") + String(sensor_data.sense_bh1750) + String("#") + String(sensor_data.sense_relay);
  espSerial.print(str);
  Serial.println(str);
  delay(300000);
}

void udara()
{
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  sensor_data.sense_dht22_hum = h;
  sensor_data.sense_dht22_temp = t;

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }
  Serial.print(F("Suhu Udara: "));
  Serial.print(t);
  Serial.println(" °C");
  Serial.print("Kelembapan Udara : ");
  Serial.print(h);
  Serial.println(" %");

  //LCD PRINT
  lcd.setCursor(0, 1);
  lcd.print("S.U:");
  lcd.setCursor(5, 1);
  lcd.print(int(t));
  lcd.print(" C");

  lcd.setCursor(0, 2);
  lcd.print("K.U:");
  lcd.setCursor(5, 2);
  lcd.print(int(h));
  lcd.print(" %");
}

void cahaya()
{
  lux = lightMeter.readLightLevel();
  sensor_data.sense_bh1750 = lux;
  Serial.print("Cahaya: ");
  Serial.print(lux);
  Serial.println(" lx");
  //LCD PRINT
  lcd.setCursor(0, 3);
  lcd.print("CAHAYA:     ");
  lcd.setCursor (8, 3);
  lcd.print(int (lux));
  lcd.setCursor (16, 3);
  lcd.print(" lux");
}

void soil()
{
  soilMoistureValue = analogRead(A0);  //put Sensor insert into soil
  //  Serial.println(soilMoistureValue);
  soilmoisturepercent = map(soilMoistureValue, AirValue, WaterValue, 0, 100);
  sensor_data.sense_soilmst = soilmoisturepercent;
  Serial.print("Kelembaoan Tanah : ");
  //  Serial.println(soilmoisturepercent);

  //  sensor_data.sense_soilmst = 47;

  if (soilmoisturepercent >= 100)
  {
    Serial.println("100 %");
    lcd.setCursor(10, 2);
    lcd.print("K.T:    ");
    lcd.setCursor(15, 2);
    lcd.print("100%");
  }
  else if (soilmoisturepercent <= 0)
  {
    Serial.println("0 %");
    lcd.setCursor(10, 2);
    lcd.print("K.T:    ");
    lcd.setCursor(15, 2);
    lcd.print("0  %");
  }
  //  else if (soilmoisturepercent > 0 && soilmoisturepercent < 100)
  else
  {
    Serial.print(soilmoisturepercent);
    Serial.println(" %");
    lcd.setCursor(10, 2);
    lcd.print("K.T:    ");
    lcd.setCursor (15, 2);
    lcd.print(soilmoisturepercent);
    lcd.print("%");
  }
}

void ds()
{
  sensors.requestTemperatures();
  Serial.print("Suhu Tanah: ");
  Serial.print(sensors.getTempCByIndex(0));
  Serial.println (" °C");
  sensor_data.sense_ds18b20 = sensors.getTempCByIndex(0);
}

void relay() {
  if (lux < THRESHOLD) {
    Serial.println("Pompa Menyala : 1");
    digitalWrite(RELAY_PIN, LOW);
    sensor_data.sense_relay = 1;
  } else {
    Serial.println("Pompa Mati : 0");
    digitalWrite(RELAY_PIN, HIGH);
    sensor_data.sense_relay = 0;
  }
}
