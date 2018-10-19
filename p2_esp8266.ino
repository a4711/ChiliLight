#include "Arduino.h"


#include "src/myiot_DeviceConfig.h"
#include "src/myiot_webServer.h"
#include "src/myiot_ota.h"
#include "src/myiot_timer_system.h"
#include "src/myiot_mqtt.h"
#include "src/TemperatureDistribution.h"
#include "LightDistribution.h"
#include "DHTDistribution.h"
#include "relay.h"

// const int D0 = 16; // GPIO16;
// const int D2 = 4; // GPIO4;
// const int D4 = 2; // GPIO2
const int LED = D4; // LED auf dem Board
// const int D5 = 14; // GPIO14,
// const int D6 = 12; // GPIO12,
static const int ONE_WIRE_PIN = D6;

OneWire oneWire(ONE_WIRE_PIN);
MyIOT::DeviceConfig config;
MyIOT::TimerSystem tsystem;
MyIOT::Mqtt mqtt;
MyIOT::OTA ota;
MyIOT::WebServer webServer;


Relay relay(D0);
TemperatureDistribution tdist(oneWire);
LightDistribution ldist;
DHTDistribution dhtdist;


//The setup function is called once at startup of the sketch
void setup()
{
	Serial.begin(76800);
	config.setup();

	ota.setup(config.getDeviceName());
	webServer.setup(config);
	mqtt.setup(config.getDeviceName(), config.getMqttServer());

	// auto xpub = [](const char* topic, const char* msg){mqtt.publish(topic, msg);};
	std::function<void(const char*, const char*)> xpub
			= std::bind(&MyIOT::Mqtt::publish, &mqtt, std::placeholders::_1, std::placeholders::_2);

	tdist.setup(xpub);
  ldist.setup(xpub);
  dhtdist.setup(xpub);

	relay.setup(xpub);
	mqtt.subscribe("cmd",[](const char*msg){
		if (0 == strcmp(msg, "on")) relay.enable(true);
		else if (0 == strcmp(msg, "off")) relay.enable(false);
		else if (0 == strcmp(msg, "toggle")) relay.toggle();
		else {
			int seconds = ::atoi(msg);
			if (seconds>0)
			{
				relay.timed_on(seconds);
			}
		}
	});


	tsystem.add(&ota, MyIOT::TimerSystem::TimeSpec(0, 100e6));  // 100ms
	tsystem.add(&webServer, MyIOT::TimerSystem::TimeSpec(0, 100e6));  // 100ms
	tsystem.add(&mqtt, MyIOT::TimerSystem::TimeSpec(0, 100e6));  // 100ms

	tsystem.add([](){tdist.expire();}, MyIOT::TimerSystem::TimeSpec(5));  // 5 s
  tsystem.add([](){ldist.expire();}, MyIOT::TimerSystem::TimeSpec(5));  // 5 s
  tsystem.add([](){dhtdist.expire();}, MyIOT::TimerSystem::TimeSpec(10));  // 5 s
	tsystem.add([](){relay.expire();}, MyIOT::TimerSystem::TimeSpec(0,10e6));  // 10ms
}

// The loop function is called in an endless loop
void loop()
{
   tsystem.run_loop(1, 1);
}
