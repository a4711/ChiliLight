#ifndef TemperatureDistribution_H_
#define TemperatureDistribution_H_

#include <functional>
#include <stdio.h>

#include <OneWire.h>
#include <DallasTemperature.h>



class TemperatureDistribution
{
  public:
    TemperatureDistribution(OneWire& aOneWire) : oneWire(aOneWire), sensors(&aOneWire){}

    void setup(std::function<void(const char*, const char*)> fpublish)
    {
      ipublish = fpublish;

      Serial.println("TemperatureDistribution::setup ... ");
      sensors.begin();
      sensors.setResolution(TEMP_12_BIT);
      Serial.print("Device Count: ");
      Serial.println(sensors.getDeviceCount());
      detect();
    }

    float get_temperature() 
    {
       sensors.requestTemperaturesByAddress(me); 
       return sensors.getTempC(me);
    }

    void publish(float temperature) const
    {
        char buffer[256];
        snprintf(buffer, sizeof(buffer)-1, "%s", String(temperature).c_str() );
        if (ipublish) ipublish("temperature",buffer);
    }

    void dump(float temperature) const
    {
        Serial.print("published ");
        Serial.print(": ");
        Serial.println(temperature);
    }

    void expire()
    {
       float temperature = get_temperature();
       publish(temperature);
       dump(temperature);
    }

    void detect()
    {
      DeviceAddress addr;
      oneWire.reset_search();
      while (oneWire.search(addr))
      {
        Serial.println("");
        if (0x28 == addr[0])
        {
          ::memcpy(me, addr, sizeof(DeviceAddress));
          printAddress(me);
          Serial.println("");
          break;
        }
      }
    }

    void printAddress(DeviceAddress deviceAddress)
    {
      for (uint8_t i = 0; i < 8; i++)
      {
        // zero pad the address if necessary
        if (deviceAddress[i] < 16) Serial.print("0");
        Serial.print(deviceAddress[i], HEX);
      }
    }
    
  private:
    std::function<void(const char*, const char*)> ipublish;

    DeviceAddress me;
    OneWire& oneWire;
    DallasTemperature sensors;
};

#endif



