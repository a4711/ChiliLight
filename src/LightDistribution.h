/*
 * LightDistribution.h
 *
 *  Created on: 29.10.2017
 *      Author: a4711
 */

#ifndef LIGHTDISTRIBUTION_H_
#define LIGHTDISTRIBUTION_H_

#include <functional>
#include <Arduino.h>


class LightDistribution {
public:
	typedef std::function<void(const char*, const char*)> FPublish;

	LightDistribution();
	virtual ~LightDistribution();

	virtual void expire();

	void setup(FPublish xpublish);

private:
	int get_light_value() const;

	void dump(int lightValue) const;
	void publish(int lightValue) const;

    FPublish ipublish;
};


#endif /* LIGHTDISTRIBUTION_H_ */
