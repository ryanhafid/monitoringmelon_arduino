#include <SoftwareSerial.h>

SoftwareSerial toNodeMcu(3, 4);

struct templeate {
  int coba;
  int coba2;
};
int size_of = sizeof(struct templeate);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  toNodeMcu.begin(1000);
  while (!Serial) continue;

}

void loop() {
  //   //put your main code here, to run repeatedly:
  //  StaticJsonDocument<500> doc;
  //
  //  doc["temperature"] = 32;
  //  doc["huminity"] = 17;
  ////  doc["lux"] = 200;
  ////  doc["moisture_soil"] = 300;
  ////  doc["temperature_DS"] = 20;
  //
  //  serializeJson(doc, toNodeMcu);
  templeate a = {4, 2};
  send(&a);
}

void send(const templeate* table) {
  toNodeMcu.write((const char*)table, size_of);
}
