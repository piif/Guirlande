#include "buttons.h"

// measured values for 3 resistor bridges with common resistor = 56k
// and 3 other ones with 56k, 120k, 200k
#if defined(ARDUINO_AVR_UNO)
int refValues[] = {
	0,
	510,
	330,
	610,
	220,
	570,
	440,
	655
};
const int tolerance = 25;
#else
int refValues[] = {
	  0,
	510,
	332,
	612,
	225,
	590,
	444,
	652
};
const int tolerance = 20;
#endif

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
//	Serial.println(value);
	return lookupButtons(value);
}
