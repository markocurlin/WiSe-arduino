#include <Arduino.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <BH1750.h>
#include <Wire.h>

#include <SPI.h>
#include <LoRa.h>

#include <WiFi.h>

#define DHTPIN 2
#define DHTTYPE DHT22

const long frequency = 868E6;

class SENSORS {
public:
        SENSORS();
        ~SENSORS();
        //void LoRa_init();
        void DHT_init();
        void BH1750_init();
        void readTempHum();
        void readLight();
};

DHT dht(DHTPIN, DHTTYPE);

BH1750 lightMeter(0x23);

SENSORS::SENSORS() {}
SENSORS::~SENSORS() {}

/*
void SENSORS::LoRa_init() {
  Serial.println(F("LoRa Sender test!"));
  LoRa.begin(frequency);
}*/

void SENSORS::DHT_init() {
    Serial.println(F("DHT test!"));
    dht.begin();
}

void SENSORS::BH1750_init() {
    lightMeter.begin(BH1750::CONTINUOUS_HIGH_RES_MODE);
    Serial.println(F("BH1750 test!"));
}

void SENSORS::readTempHum() {
    float humidity = dht.readHumidity();
    float temperature = dht.readTemperature();

    if (isnan(humidity) || isnan(temperature)) {
        Serial.println(F("Failed to read from DHT sensor!"));
        return;
    }

    Serial.println(F("Humidity: "));
    Serial.println(humidity);
    Serial.println(F(" %/t"));
    Serial.println(F("Temperature: "));
    Serial.println(temperature);
}

void SENSORS::readLight() {
    uint32_t lux = lightMeter.readLightLevel();

    Serial.println(F("Light: "));
    Serial.println(lux);
    Serial.println(F(" lx"));
}

SENSORS sensor;

void setup() {
  Serial.begin(115200);
  //sensor.LoRa_init();
  sensor.DHT_init();
  sensor.BH1750_init();
  delay(100);
}

void loop() {
  sensor.readTempHum();
  sensor.readLight();
}







/*
#include "Sensors.cpp"
#include "Sensors.h"

SENSORS sensor;

void setup() {
  Serial.begin(115200);
  //sensor.DHT_init();
  //sensor.BH1750_init();
  delay(100);
}

void loop() {
  //sensor.readTempHum();
  //sensor.readLight();
}


*/