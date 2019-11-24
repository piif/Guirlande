#include "Arduino.h"

#ifdef ARDUINO_attiny

#define BUTTONS_INPUT A1
#define LED_1 0
#define LED_2 1
#define LED_3 4

#else

#define BUTTONS_INPUT A1
#define LED_1 2
#define LED_2 3
#define LED_3 4

#endif

const int tolerance = 15;

// computed values for 3 resistor bridges with common resistor = R
// and 3 other ones with R, 2*R, 3*R
int refValues[] = {
	0,
	512,
	341,
	614,
	256,
	585,
	465,
	663
};
const byte nbValues = (sizeof(refValues)/sizeof(int));

byte lookup(int value) {
	for(byte i=0; i < nbValues; i++) {
		if (value >= refValues[i]-tolerance && value <= refValues[i]+tolerance) {
			return i;
		}
	}
	return 255;
}

void setup() {
#ifndef ARDUINO_attiny
	Serial.begin(115200);
#endif

	pinMode(LED_1, OUTPUT);
	pinMode(LED_2, OUTPUT);
	pinMode(LED_3, OUTPUT);

//	pinMode(BUTTONS_INPUT, INPUT);
}

byte buttons = 0;

void loop() {
#ifndef ARDUINO_attiny
	delay(500);
#endif
	int value = analogRead(BUTTONS_INPUT);
	byte newButtons = lookup(value);

	if (newButtons != 255 && newButtons != buttons) {
		buttons = newButtons;
#ifndef ARDUINO_attiny
		Serial.print(value);
		Serial.print(" => ");
		Serial.println(buttons);
#endif
		digitalWrite(LED_1, !(buttons & 1));
		digitalWrite(LED_2, !(buttons & 2));
		digitalWrite(LED_3, !(buttons & 4));
	}
}
