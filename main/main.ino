#include <ESP8266WiFi.h>

#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

#include <time.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

#include <MQTT.h>
#include <MQTTClient.h>

#include "aws_cred_private.h"
//private file isn't (hopefully) committed. See Template to make your own.
//#include "aws_cred_template.h"

#include "wifi_cred_private.h"
//private file isn't (hopefully) committed. See Template to make your own.
//#include "wifi_cred_template.h"

#define dhtDataPin D1
#define DHTTYPE DHT11

DHT dht(dhtDataPin, DHTTYPE);

float t = 0.0;
float h = 0.0;

WiFiClientSecure net = WiFiClientSecure();
MQTTClient client = MQTTClient(512);

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);

// the setup routine runs once when you press reset:
void setup() {
  // initialize serial communication at 9600 bits per second:
  Serial.begin(115200);
  delay(1000);

  pinMode(D2, OUTPUT);
  pinMode(D3, OUTPUT);

  Serial.print("\nStarting Wifi");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print("w");
  }
  Serial.println(".");
  Serial.print("Connected, IP address: ");
  Serial.println(WiFi.localIP());

  Serial.print("Connecting NTP");
  timeClient.begin();

  while(!timeClient.update()) {
    timeClient.forceUpdate();
    Serial.print("t");
  }
  Serial.println(timeClient.getFormattedTime());
  net.setX509Time(timeClient.getEpochTime());
  Serial.println(".");

  Serial.print("\nconnecting AWS IOT...");

  net.setBufferSizes(512, 512);
  
  BearSSL::X509List caCert(AWS_CERT_CA);
  net.setTrustAnchors(&caCert);

  BearSSL::X509List clientCert(AWS_CERT_CRT);
  BearSSL::PrivateKey clientKey(AWS_CERT_PRIVATE);
  net.setClientRSACert(&clientCert, &clientKey);
  
  client.begin(AWS_IOT_ENDPOINT, AWS_IOT_PORT, net);
  while (!client.connect(DEVICE_NAME)) {
    Serial.println("MQTT connect failed");
      
    char buf[256];
    net.getLastSSLError(buf,256);
    Serial.print("WiFiClientSecure SSL error: ");
    Serial.println(buf);
    Serial.println(".");
    delay(5000);
  }
  Serial.print("AWS IOT Connected");
}

// the loop routine runs over and over again forever:
void loop() { 
  if(WiFi.status() != WL_CONNECTED) {
   while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(";");
  } 
    Serial.println(",");
    Serial.print("Connected, IP address: ");
    Serial.println(WiFi.localIP());
  }
  Serial.println(",");

  digitalWrite(D2, HIGH);
  digitalWrite(D3, HIGH);
  // read the input on analog pin 0:
  int sensorValue = analogRead(A0);
  // print out the value you read:
  Serial.println("ldr");
  Serial.println(sensorValue);

  Serial.println("dht");
  delay(2000);
  dht.begin();
  float newT = dht.readTemperature();
  if(isnan(newT)) {
      Serial.println("Failed to read from DHT");
  }
  else {
    Serial.println(newT);
  }
  float newH = dht.readHumidity();
  if(isnan(newH)) {
    Serial.println("Failed to read humidity from DHT");
  }
  else {
    Serial.println(newH);
  }
  
  digitalWrite(D2, LOW);
  digitalWrite(D3, LOW);
  
  const size_t capacity = JSON_OBJECT_SIZE(20);
  DynamicJsonDocument doc(capacity);

  doc["light"] = sensorValue;
  doc["humidity"] = newH;
  doc["temperature"] = newT;
  
  StaticJsonDocument<512> jsonDoc;
  JsonObject stateObj = jsonDoc.createNestedObject("state");
  JsonObject reportedObj = stateObj.createNestedObject("reported");

  reportedObj["message"] = "garden_sensor";
  reportedObj["light"] = sensorValue;
  reportedObj["temp"] = newT;
  reportedObj["humidity"] = newH;

  char jsonBuffer[512];
  serializeJson(jsonDoc, jsonBuffer);
  Serial.println(jsonBuffer);
  client.publish(AWS_IOT_TOPIC, jsonBuffer);

  client.disconnect();
  Serial.println("Deep sleeping");
  ESP.deepSleep(30 * 60 * 1e6);
}
