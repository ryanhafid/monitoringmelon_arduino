#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <SoftwareSerial.h>

// WiFi
const char *ssid = "HAN"; // Enter your WiFi name
const char *password = "Yc2jct2000";  // Enter WiFi password

// MQTT Broker
const char *mqtt_broker = "192.168.0.109";
const char *topic = "sensor";
const int mqtt_port = 1883;
String x = "";

WiFiClient espClient;
PubSubClient client(espClient);
SoftwareSerial fromArduino(D6, D7); //RX, TX

void recon() {
  while (!client.connected()) {
    String client_id = "esp8266-client-";
    client_id += String(WiFi.macAddress());
    Serial.printf("The client %s connects to the public mqtt broker\n", client_id.c_str());
    if (client.connect(client_id.c_str())) {
      Serial.println("Public emqx mqtt broker connected");
    } else {
      Serial.print("failed with state ");
      Serial.print(client.state());
      delay(2000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  fromArduino.begin(9600);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to WiFi..");
  }
  Serial.println("Connected to the WiFi network");
  //connecting to a mqtt broker
  client.setServer(mqtt_broker, mqtt_port);
  client.setCallback(callback);

  recon();
}

void callback(char *topic, byte *payload, unsigned int length) {
  Serial.print("Message arrived in topic: ");
  Serial.println(topic);
  Serial.print("Message:");
  for (int i = 0; i < length; i++) {
    Serial.print((char) payload[i]);
  }
  Serial.println();
  Serial.println("-----------------------");
}

void loop() {

  if (!client.connected()) {
    recon();
  }
  
  if (fromArduino.available()) {
    x = fromArduino.readString();
    Serial.println(x);
    client.publish("sensor", x.c_str());
  }
  client.loop();
  delay(2000);
}
