//https://sandervandevelde.wordpress.com/2019/05/07/connection-a-cheap-esp8266-to-azure-iot-central/
//http://blogs.recneps.org/post/Connecting-the-ESP-8266-to-Azure-IoT-Hub-using-MQTT-and-MicroPython

//wifi: https://arduino-esp8266.readthedocs.io/en/latest/esp8266wifi/readme.html
//https://iotdesignpro.com/projects/how-to-connect-esp8266-with-mqtt
#include <ESP8266WiFi.h>

// using wemos d1 mini esp8266 clone
//https://diyi0t.com/how-to-reduce-the-esp8266-power-consumption/

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
// https://randomnerdtutorials.com/esp8266-dht11dht22-temperature-and-humidity-web-server-with-arduino-ide

DHT dht(dhtDataPin, DHTTYPE);

float t = 0.0;
float h = 0.0;

//https://itsmerajasekar.medium.com/getting-started-with-iot-using-esp8266-node-mcu-and-azure-iot-hub-37a3ca03dd56
// ssl example: https://github.com/esp8266/Arduino/blob/master/libraries/ESP8266WiFi/examples/BearSSL_ServerClientCert/BearSSL_ServerClientCert.ino

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

  //https://arduino-esp8266.readthedocs.io/en/latest/esp8266wifi/bearssl-client-secure-class.html
  // Important! NTP required prior to doing SSL handshake!
  // https://raphberube.com/making-the-esp-8266-work-with-aws-io-t
  // https://gist.github.com/raph84/ff1fc1896c58b7f50125bda23dee17ae

// TODO: Read... 
//https://medium.com/accenture-the-dock/esp8266-aws-iot-core-guide-c640f2622a51
//https://github.com/esp8266/Arduino/issues/5347
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
  // woah, saved me: https://forum.arduino.cc/t/ide-creates-an-error-while-trying-to-upload-a-sketch-to-esp8266-deepsleep/589213/4
  ESP.deepSleep(30 * 60 * 1e6);
}
