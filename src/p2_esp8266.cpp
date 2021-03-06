#include "Arduino.h"


#include "myiot_DeviceConfig.h"
#include "myiot_webServer.h"
#include "myiot_ota.h"
#include "myiot_timer_system.h"
#include "myiot_mqtt.h"
#include "TemperatureDistribution.h"
#include "LightDistribution.h"
#include "DHTDistribution.h"
#include "relay.h"
#include "Bme280.h"
#include <FastLED.h>

// const int D0 = 16; // GPIO16;
// const int D1 = 5; // GPIO5;
// const int D2 = 4; // GPIO4;
// const int D4 = 2; // GPIO2
const int LED = D4; // LED auf dem Board
// const int D5 = 14; // GPIO14,
// const int D6 = 12; // GPIO12,
// const int D7 = 13; // GPIO12,
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
Bme280 bme280;



void setup_system()
{
	config.setup();

	ota.setup(config.getDeviceName());
	webServer.setup(config);
	mqtt.setup(config.getDeviceName(), config.getMqttServer());

	tsystem.add(&ota, MyIOT::TimerSystem::TimeSpec(0, 100e6));  // 100ms
	tsystem.add(&webServer, MyIOT::TimerSystem::TimeSpec(0, 100e6));  // 100ms
	tsystem.add(&mqtt, MyIOT::TimerSystem::TimeSpec(0, 100e6));  // 100ms
}


CRGB leds[1];

void setup_status_led()
{
	FastLED.addLeds<WS2812B, 13, GRB>(leds, 1); // strange: use the nodeMCU number to D7 to get output on GPIO13
	leds[0] =  CRGB::Cyan;
	FastLED.setBrightness(30);
	FastLED.show();

	mqtt.subscribe("rgb", [](const char*msg)
	{
		leds[0] = ::atoi(msg);
		FastLED.show();
	});
}


//The setup function is called once at startup of the sketch
void setup()
{
	Serial.begin(76800);
	setup_system();

	// auto xpub = [](const char* topic, const char* msg){mqtt.publish(topic, msg);};
	std::function<void(const char*, const char*)> xpub
			= std::bind(&MyIOT::Mqtt::publish, &mqtt, std::placeholders::_1, std::placeholders::_2);

	tdist.setup(xpub);
	ldist.setup(xpub);
	dhtdist.setup(xpub);
	bme280.setup(xpub);

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

	tsystem.add([](){tdist.expire();}, MyIOT::TimerSystem::TimeSpec(5));  // 5 s
	tsystem.add([](){ldist.expire();}, MyIOT::TimerSystem::TimeSpec(5));  // 5 s
	tsystem.add([](){dhtdist.expire();}, MyIOT::TimerSystem::TimeSpec(10));  // 10 s
	tsystem.add([](){relay.expire();}, MyIOT::TimerSystem::TimeSpec(0,10e6));  // 10ms
	tsystem.add([](){bme280.expire();}, MyIOT::TimerSystem::TimeSpec(300));  // 300 s (-> 5 minutes)

	setup_status_led();
}

// The loop function is called in an endless loop
void loop()
{
   tsystem.run_loop(1, 1);
}
