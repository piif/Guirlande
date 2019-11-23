#include "Arduino.h"

void setup() {
	Serial.begin(115200);
	pinMode(13, OUTPUT);
	digitalWrite(13, LOW);

	pinMode(A0, INPUT);
}

const int threshold = 150;
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
//	if (value < threshold) {
//		return 0;
//	}
	for(byte i=0; i < nbValues; i++) {
		if (value >= refValues[i]-tolerance && value <= refValues[i]+tolerance) {
			return i;
		}
	}
	return 255;
}

void loop() {
	delay(500);
	int value = analogRead(A0);
	Serial.print(value);
	Serial.print(" => ");
	Serial.println(lookup(value));
}
