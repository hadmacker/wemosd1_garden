# wemosd1_garden

## Libraries Used

* [NTPClient](https://github.com/arduino-libraries/NTPClient) by Fabrice Weinberg, 3.2.0
* [WiFi](https://www.arduino.cc/en/Reference/WiFi) Built-In by Arduino, 1.2.7
* [Adafruit Unified Sensor](https://github.com/adafruit/Adafruit_Sensor) by Adafruit, 1.1.4
* [ArduinoJson](https://arduinojson.org/?utm_source=meta&utm_medium=library.properties) by Benoit Blanchon, 6.18.0
* [ArduinoMqtt](https://github.com/monstrenyatko/ArduinoMqtt) by Oleg Kovalenko, 1.5.1
  * Unclear if this library is currently used, I think that the alternate MQTT library is used instead.
* [MQTT](https://github.com/256dpi/arduino-mqtt) by Joel Gaehwiler, 2.5.0
* [PubSubClient](https://pubsubclient.knolleary.net/) by Nick O'Leary, 2.8.0
  * This library is installed as a library. Download the .ZIP file and install in the Arduino IDE using "Include Library..., Add ZIP Library".

## Hardware Configuration

* D0 <-> RST (for ESP8266 Deep Sleep)
* D1 <-> DHT11.Signal
* D2 <-> DHT11.Vcc
  * In an effort to reduce power consumption, D3 will only be powered up prior to performing LDR measurement, then immediately set LOW.
* D3 <-> LDR.Vcc
  * In an effort to reduce power consumption, D3 will only be powered up prior to performing LDR measurement, then immediately set LOW.
* A0 <-> LDR.Signal
* GND <-> LDR.GND
* GND <-> DHT11.GND

## Primary Hardware

* *Wemos D1 Mini (ESP8266)*
  * [AliExpress](https://www.aliexpress.com/item/1005002678530052.html?spm=a2g0o.store_pc_allProduct.8148356.8.3a7e3031ZAkyxa)
  * Alternatively, look at the [ESP32 DevKitC](https://www.espressif.com/en/products/devkits/esp32-devkitc/overview)
    * [AliExpress](https://www.aliexpress.com/item/4000103411061.html?spm=a2g0s.9042311.0.0.27424c4d9SjIi2)
    * ESP32-WROOM-32D: has embedded Wifi antenna
    * ESP32-WROOM-32U: requires an external Wifi antenna using U.FL / IPEX connection (not included).
* *DHT 11 Digital Temperator and Humidity Sensor*
  * [AliExpress](https://www.aliexpress.com/item/32840892862.html?spm=a2g0o.store_pc_allProduct.8148356.2.3d2155097XIpLK)
  * [ESP8266 Reference Implementation](https://randomnerdtutorials.com/esp8266-dht11dht22-temperature-and-humidity-web-server-with-arduino-ide)
* *Light Sensor Module, also known as a Photoresistor, also known as a Light Dependent Resistor*
  * [Light Sensor Module](https://www.osepp.com/electronic-modules/sensor-modules/69-light-sensor-module)
  * Using a sensor module, you won't need to add a separate 10k resistor for a voltage divider or pull-down.

## Supporting Hardware

Depending on your intended purpose, you may want to include...

* 2x Breadboards
  * While you can technically get by with a single breadboard, the ESP8266 and ESP32 have wide form facters that take up many of the breadboarding pins. *Configure your power buses down the center* and put one breadboard on either side of the ESP* pins. Makes development so much more convenient when breadboarding.
* Resistors, specifically 100 ohm up to 10k ohm. Useful for pull-up/downs and LED resistors, but do some research first as pins may also have built-in pull-up/downs.
* Optional, [Female Pin Header Connector Strips](https://www.aliexpress.com/item/32352375802.html?spm=a2g0s.9042311.0.0.27074c4dDgz9uZ) can be useful to make your configurations less permanent.
* Solid core wire, solder, prototyping PCB's, etc...

## Primary Resources

The following list of resources was directly used in the completion of this project:

* *MQTT*
  * [BearSSL ServerClientCert](https://github.com/esp8266/Arduino/blob/master/libraries/ESP8266WiFi/examples/BearSSL_ServerClientCert/BearSSL_ServerClientCert.ino)
    * *NTP sync is required for BearSSL certificate verification*. I skipped this step at first and nothing worked. 
      * This makes sense... certificates are only valid for designated timeframes.
    * Wait... _BearSSL_?? Didn't WiFiSecureclient use [AxTLS](https://github.com/esp8266/Arduino/issues/4826#issuecomment-398544397)?
  * [BearSSL WiFiSecureClient Methods](https://arduino-esp8266.readthedocs.io/en/latest/esp8266wifi/bearssl-client-secure-class.html)
  * [Making the ESP8266 work with AWS IoT](https://raphberube.com/making-the-esp-8266-work-with-aws-io-t)
    * [Gist: Publishing MQTT messages](https://gist.github.com/raph84/ff1fc1896c58b7f50125bda23dee17ae)
  * [How to connect ESP8266 with MQTT](https://iotdesignpro.com/projects/how-to-connect-esp8266-with-mqtt)
* [Wifi](https://arduino-esp8266.readthedocs.io/en/latest/esp8266wifi/readme.html)
* [How to reduce ESP8266 power consumption](https://diyi0t.com/how-to-reduce-the-esp8266-power-consumption/)
* [Unable to upload firmware on an ESP8266 in Deep Sleep](https://forum.arduino.cc/t/ide-creates-an-error-while-trying-to-upload-a-sketch-to-esp8266-deepsleep/589213/4)
  * tl;dr: D0 to RST is required to force and wake from deep sleep. Disconnect this connection and reset the MCU to upload new sketch.

## Other Resources

* [Microsoft IoT For Beginners](https://github.com/microsoft/IoT-For-Beginners)
  * Not used directly for this project, but if you're hungry for more IoT, this would be the path I recommend. If you get hands-on, let me know how it goes for you!

While this project ended up using AWS IoT Core, my initial path started with Azure IoT Central... which failed for one reason or another. My early links regarding Azure IoT are listed below in case you're curious about this path. Hopefully you can solve it where I failed!

* [Getting started with IoT using ESP8266 Node MCU and Azure IoT Hub](https://itsmerajasekar.medium.com/getting-started-with-iot-using-esp8266-node-mcu-and-azure-iot-hub-37a3ca03dd56)
* [Connecting a cheap ESP8266 to Azure IoT Hub](https://sandervandevelde.wordpress.com/2019/05/07/connection-a-cheap-esp8266-to-azure-iot-central/)
* [Connecting the ESP-8266 to Azure IoT Hub using MQTT and MicroPython](http://blogs.recneps.org/post/Connecting-the-ESP-8266-to-Azure-IoT-Hub-using-MQTT-and-MicroPython)
