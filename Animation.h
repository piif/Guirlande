#ifndef ANIMATION_H
#define ANIMATION_H
#include <Arduino.h>
#include "myStrip.h"

class Animation {
public:
	// execute step of animation
	// returns number of milliseconds to wait before next call
	virtual long doStep(long step) {
		Serial.println("doStep virtual !!");
		return 0;
	};

	virtual void status() {
		Serial.println("status virtual !!");
	};
	virtual ~Animation() {};
};

#endif
