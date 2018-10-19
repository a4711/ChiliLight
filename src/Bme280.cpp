/*
 * Bme280.cpp
 *
 *  Created on: 19.10.2018
 *      Author: a4711
 */

#include "Bme280.h"
#include <Arduino.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

Bme280::Bme280 (): bme(nullptr)
{
  bme = new Adafruit_BME280();
  if (nullptr == bme)
  {
    Serial.println("Bme280 ERROR: can't create Adafruit_BME280 instance.");
  }
}

Bme280::~Bme280 ()
{
}

void Bme280::setup(const FPublish& fpublish)
{
  if (nullptr == bme)
  {
    Serial.println("Bme280::setup ERROR: no Adafruit_BME280 instance.");
    return;
  }

  ipublish = fpublish;
  Serial.println("Bme280::setup ... ");
  bool status = bme->begin();
  if (!status) {
      Serial.println("Could not find a valid BME280 sensor, check wiring!");
      while (1);
  }
}

void Bme280::internal_publish()
{
  if (nullptr == bme)
  {
    Serial.println("Bme280::setup ERROR: no Adafruit_BME280 instance.");
    return;
  }

  float t = bme->readTemperature();
  float p = bme->readPressure() / 100.0F;
  float h = bme->readHumidity();
  String st(t);
  String sp(p);
  String sh(h);

  char message [200];
  snprintf(message, sizeof(message), "{\"temperature\":\"%s\", \"humidity\":\"%s\", \"pressure\":\"%s\"}", st.c_str(), sh.c_str(), sp.c_str());
  Serial.print("bme280: "); Serial.println(message);
  if (ipublish)
  {
    ipublish("bme280", message);
  }

}

