#include "buttons.h"

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
const int tolerance = 15;
const byte nbValues = (sizeof(refValues)/sizeof(int));

byte lookupButtons(int value) {
	for(byte i=0; i < nbValues; i++) {
		if (value >= refValues[i]-tolerance && value <= refValues[i]+tolerance) {
			return i;
		}
	}
	return 255;
}

byte readButtons() {
	int value = analogRead(BUTTON_INPUT);
	return lookupButtons(value);
}
