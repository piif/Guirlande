#include "Arduino.h"

// uncomment to force all lights to max intensity
// #define POWER_TEST

#include "myStrip.h"
#include "myLine.h"
#include "buttons.h"

#ifndef DEFAULT_BAUDRATE
	#define DEFAULT_BAUDRATE 115200
#endif

#include "StripEffects.h"
#include "LineEffects.h"

#ifndef ARDUINO_attiny
void usage() {
	Serial.println("? => this help");
	Serial.println("+ => light on");
	Serial.println("- => light off");
	Serial.println("0..9 => force to loop on this line effect");
	Serial.println("A..Z => force to loop on this strip effect");
	Serial.println(". => iterate on effects");
	Serial.flush();
}
#endif

void setup(void) {
#ifndef ARDUINO_attiny
	Serial.begin(DEFAULT_BAUDRATE);
#endif

	stripInit(STRIP_LEN, STRIP_PIN);
	lineInit();

	stripUpdate(); // Initialize all pixels to 'off'

#ifndef ARDUINO_attiny
	usage();
	Serial.println("ready"); Serial.flush();
#endif
}

const unsigned int nbLineEffects = sizeof(lineEffects) / sizeof(stripEffectFunction);
short effectLine = 1;
short forcedLine = -1;
unsigned long stepLine = 0;
long delayLine = 0;

const unsigned int nbEffectsStrip = sizeof(effectsStrip) / sizeof(stripEffectFunction);
short effectStrip = 1;
short forcedStrip = -1;
unsigned long stepStrip = 0;
long delayStrip = 0;

byte buttons = 0;
bool lineDisabled = false, stripDisabled = false, testMode = false;

#ifndef ARDUINO_attiny
void status() {
	Serial.print("Status :");
	Serial.print(lineDisabled ? " line off" : " line on");
	Serial.println(stripDisabled ? ", strip off" : ", strip on");
	if (stripDisabled && lineDisabled) {
		return;
	}
	Serial.print("Line : anim "); Serial.print(effectLine);
	Serial.print(" step "); Serial.println(stepLine);
	Serial.println();
	Serial.print("Strip : anim "); Serial.print(effectStrip);
	Serial.print(" step "); Serial.println(stepStrip);
	Serial.println();
}
#endif

void handleLine() {
	if (delayLine == 0) {
		if (stepLine == 0) {
			lineSet(0);
		}
		delayLine = lineEffects[forcedLine == -1 ? effectLine : forcedLine](stepLine, leds);
		if (delayLine == -1) {
			stepLine = 0;
			if (forcedLine == -1) {
				effectLine++;
				if (effectLine >= nbLineEffects) {
					effectLine = 1;
				}
			}
			delayLine = 0;
		} else {
			stepLine++;
		}
	} else {
		delayLine--;
	}
	displayStep();
}

void handleStrip() {
	if (delayStrip > 0) {
		// wait for next iteration
		delayStrip--;
	} else {
		if (stepStrip == 0) {
			stripOff();
		}
		delayStrip = effectsStrip[forcedStrip == -1 ? effectStrip : forcedStrip](stepStrip);
		if (delayStrip < 0) {
			delayStrip = 0;
			stepStrip = 0;
			if (forcedStrip == -1) {
				effectStrip++;
				if (effectStrip >= nbEffectsStrip) {
					effectStrip = 1;
				}
			}
		} else {
			stepStrip++;
		}
	}
}


void forceLine(short forced) {
	delayLine = stepLine = 0;
	forcedLine = forced;
	handleLine();
}

void forceStrip(short forced) {
	delayStrip = stepStrip = 0;
	forcedStrip = forced;
	handleStrip();
}

void handleButtons(byte newButtons) {
#ifndef ARDUINO_attiny
	Serial.print("handle buttons "); Serial.println(newButtons);
#endif
	if ((buttons & BUTTON_LINE) != (newButtons & BUTTON_LINE)) {
		lineDisabled = newButtons & BUTTON_LINE;
		if (lineDisabled) {
			lineOff();
		}
	}
	if ((buttons & BUTTON_STRIP) != (newButtons & BUTTON_STRIP)) {
		stripDisabled = newButtons & BUTTON_STRIP;
		if (stripDisabled) {
			stripOff();
		}
	}
	if ((buttons & BUTTON_TEST) != (newButtons & BUTTON_TEST)) {
		testMode = newButtons & BUTTON_TEST;
		if (testMode) {
			forceLine(0);
			forceStrip(0);
		} else {
			forcedLine = -1;
			forcedStrip = -1;
		}
	}
	buttons = newButtons;
#ifndef ARDUINO_attiny
	status();
#endif
}

#ifndef ARDUINO_attiny
void checkInput() {
	if (Serial.available()) {
		int b = Serial.read();
		if (b == '-') {
			lineDisabled = stripDisabled = true;
			stripOff();
			lineOff();
		} else if (b == '+') {
			lineDisabled = stripDisabled = false;
			stepLine = effectLine = 0;
			effectStrip = stepStrip = 0;
		} else if (b >= '0' && b <= '9') {
			forceLine((b - '0') % nbLineEffects);
		} else if (b >= 'A' && b <= 'Z') {
			forceStrip((b - 'A') % nbEffectsStrip);
		} else if (b == '.') {
			forcedLine = -1;
			forcedStrip = -1;
		} else if (b == 'Disabled?') {
			usage();
			status();
		}
	}
}
#endif

void loop() {
#ifndef ARDUINO_attiny
	checkInput();
#endif
	byte newButtons = readButtons();
	if (newButtons != buttons && newButtons != 255) {
		handleButtons(newButtons);
	}
	if (!lineDisabled) {
		handleLine();
	}
	if (!stripDisabled) {
		handleStrip();
	}

#ifndef ARDUINO_attiny
	delay(1);
#endif
}
