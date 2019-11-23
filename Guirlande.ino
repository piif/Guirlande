#include "Arduino.h"

// uncomment to force all lights to max intensity
// #define POWER_TEST

#include "myStrip.h"
#include "myLine.h"

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
	stripUpdate(); // Initialize all pixels to 'off'

	lineInit();

#ifndef ARDUINO_attiny
	usage();
	Serial.println("ready"); Serial.flush();
#endif
}

const unsigned int nbLineEffects = sizeof(lineEffects) / sizeof(stripEffectFunction);
short effectLine = 0;
short forcedLine = -1;
unsigned long stepLine = 0;
long delayLine = 0;

const unsigned int nbEffectsStrip = sizeof(effectsStrip) / sizeof(stripEffectFunction);
short effectStrip = 0;
short forcedStrip = -1;
unsigned long stepStrip = 0;
long delayStrip = 0;

bool off = false;

#ifndef ARDUINO_attiny
void status() {
	Serial.print("Status "); Serial.println(off ? "off" : "on");
	if (off) {
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

#ifndef ARDUINO_attiny
void checkInput() {
	if (Serial.available()) {
		int b = Serial.read();
		if (b == '-') {
			off = true;
			stripOff();
			digitalWrite(8, LOW);
		} else if (b == '+') {
			digitalWrite(8, HIGH);
			off = false;
			stepLine = effectLine = 0;
			effectStrip = stepStrip = 0;
		} else if (b >= '0' && b <= '9') {
			forcedLine = (b - '0') % nbLineEffects;
			effectLine = forcedLine;
			stepLine = 0;
		} else if (b >= 'A' && b <= 'Z') {
			forcedStrip = (b - 'A') % nbEffectsStrip;
			effectStrip = forcedStrip;
			stepStrip = 0;
		} else if (b == '.') {
			forcedLine = -1;
			forcedStrip = -1;
		} else if (b == '?') {
			usage();
			status();
		}
	}
}
#endif

void handleLine() {
	if (delayLine == 0) {
		if (stepLine == 0) {
			lineSet(0);
		}
		delayLine = lineEffects[effectLine](stepLine, leds);
		if (delayLine == -1) {
			stepLine = 0;
			if (forcedLine == -1) {
				effectLine++;
				if (effectLine >= nbLineEffects) {
					effectLine = 0;
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
		delayStrip = effectsStrip[effectStrip](stepStrip);
		if (delayStrip < 0) {
			delayStrip = 0;
			stepStrip = 0;
			if (forcedStrip == -1) {
				effectStrip++;
				if (effectStrip >= nbEffectsStrip) {
					effectStrip = 0;
				}
			}
		} else {
			stepStrip++;
		}
	}
}

void loop() {
#ifndef ARDUINO_attiny
	checkInput();
#endif
	if (!off) {
		handleLine();
		handleStrip();
	}

	delay(1);
}
