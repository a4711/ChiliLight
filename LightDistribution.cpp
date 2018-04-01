/*
 * LightDistribution.cpp
 *
 *  Created on: 29.10.2017
 *      Author: a4711
 */

#include "LightDistribution.h"

LightDistribution::LightDistribution() {
}

LightDistribution::~LightDistribution() {
}


void LightDistribution::expire()
{
    int licht = get_light_value();
   	publish(licht);
    dump(licht);
}


void LightDistribution::setup(FPublish xpublish) {
	ipublish = xpublish;
}

int LightDistribution::get_light_value() const {
	return analogRead(A0);
}

void LightDistribution::dump(int lightValue) const {
	Serial.print("published ");
	Serial.print("lightvalue");
	Serial.print(" = ");
	Serial.println(lightValue);
}

void LightDistribution::publish(int lightValue) const {
	char buffer[256];
	snprintf(buffer, sizeof(buffer) - 1, "%d", lightValue);
	if (ipublish)
		ipublish("lightvalue", buffer);
}
