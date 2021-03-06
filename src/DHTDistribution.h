/*
 * DHTDistribution.h
 *
 *  Created on: 24.03.2018
 *      Author: a4711
 */

#ifndef DHTDISTRIBUTION_H_
#define DHTDISTRIBUTION_H_
#include <SimpleDHT.h>
#include <functional>

class DHTDistribution
{
  enum Constants
  {
    D5 = 14,
    OneWire = D5,
  };

public:
  DHTDistribution ();
  virtual
  ~DHTDistribution ();

  void expire()
  {
    if (measure())
    {
      publish();
    }
  }
  void setup(std::function<void(const char*, const char*)> fpublish)
  {
    ipublish = fpublish;
  }


private:
  void publish()
  {
    char message [200];
    String sh(currentHumidity);
    String st(currentTemperature);

    Serial.print("Temperature: "); Serial.print(st.c_str());
    Serial.print("Humidity: "); Serial.print(sh.c_str());
    Serial.println();

    if (ipublish)
    {
      snprintf(message, sizeof(message), "{\"temperature\":\"%s\", \"humidity\":\"%s\"}", st.c_str(), sh.c_str());
      ipublish("dht", message);
    }
  }

  void publish_error(const char* message)
  {
    if (ipublish)
    {
      ipublish("dht_error", message);
    }
  }

  bool measure()
  {
    currentHumidity = 0.0;
    currentTemperature = 0.0;
    bool retVal = true;

    unsigned long start = millis();
    int err = SimpleDHTErrSuccess;
    if ((err = dht11.read2(OneWire, &currentTemperature, &currentHumidity, NULL)) != SimpleDHTErrSuccess)
    {
      String msg = "Read DHT11 failed, err=0x" + String(err, 16);
      Serial.println(msg.c_str());
      publish_error(msg.c_str());
      retVal = false;
    }
    unsigned long dt = millis() - start;

    if (95.0 == currentHumidity)
    {
      const char* msg = "Read DHT11 failed: measure is 95�";
      Serial.print(msg);
      publish_error(msg);
      retVal = false;
    }

    Serial.print("measuring dht took "); Serial.print(dt); Serial.println(" ms");
    return retVal;
  }

  float currentHumidity = 0.0;
  float currentTemperature = 0.0;
  SimpleDHT11 dht11;
  std::function<void(const char*, const char*)> ipublish;
};

#endif /* DHTDISTRIBUTION_H_ */
