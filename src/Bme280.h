/*
 * Bme280.h
 *
 *  Created on: 19.10.2018
 *      Author: a4711
 */

#ifndef SRC_BME280_H_
#define SRC_BME280_H_

#include <functional>

class Adafruit_BME280;

class Bme280
{
public:
  Bme280 ();
  virtual ~Bme280 ();

  typedef std::function<void(const char*, const char*)> FPublish;

  void setup(const FPublish& fpublish);

  void expire(){internal_publish();}

private:
  void internal_publish();

  Adafruit_BME280* bme;
  FPublish ipublish;
};

#endif /* SRC_BME280_H_ */
