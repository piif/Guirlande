#include "Arduino.h"

#define LINE_ENABLED
#define STRIP_ENABLED
/**
 * Electronic part : see fritzing file in schemas directory
 *
 * For Arduino Uno :
 * - LED Strip data pin on A0      (defined in myStrip.h)
 * - LED line A,B,C on pin 9,10,11 (defined in myLine.h)
 * - buttons on A1                 (defined in buttons.h)
 *
 * For ATtiny :
 *
 *        RST  1|o   |8   VCC
 * strip data  2|    |7   buttons
 *     line C  3|    |6   line B
 *        GND  4|    |5   line A
 *
 */
#ifdef STRIP_ENABLED
#include "myStrip.h"
#endif
#ifdef LINE_ENABLED
#include "myLine.h"
#endif
#include "buttons.h"

#ifndef DEFAULT_BAUDRATE
	#define DEFAULT_BAUDRATE 115200
#endif

#ifdef STRIP_ENABLED
#include "StripEffects.h"
#endif
#ifdef LINE_ENABLED
#include "LineEffects.h"
#endif

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

#ifdef STRIP_ENABLED
	stripInit(STRIP_LEN, STRIP_PIN);
	stripUpdate(); // Initialize all pixels to 'off'
#endif
#ifdef LINE_ENABLED
	lineInit();
#endif

#ifndef ARDUINO_attiny
	usage();
	Serial.println("ready"); Serial.flush();
#endif
}

byte buttons = 0;
bool testMode = false;

#ifdef LINE_ENABLED
bool lineDisabled = false;
const unsigned int nbLineEffects = sizeof(lineEffects) / sizeof(lineEffectFunction);
short effectLine = 1;
short forcedLine = -1;
unsigned long stepLine = 0;
long delayLine = 0;
#endif

#ifdef STRIP_ENABLED
bool stripDisabled = false;
const unsigned int nbEffectsStrip = sizeof(effectsStrip) / sizeof(stripEffectFunction);
short effectStrip = 1;
short forcedStrip = -1;
unsigned long stepStrip = 0;
long delayStrip = 0;
#endif

#ifndef ARDUINO_attiny
void status() {
	Serial.print("Status :");
#ifdef LINE_ENABLED
	Serial.print(lineDisabled ? " line off" : " line on");
#endif
#ifdef STRIP_ENABLED
	Serial.println(stripDisabled ? ", strip off" : ", strip on");
#endif
#ifdef LINE_ENABLED
	Serial.print("Line : anim "); Serial.print(effectLine);
	Serial.print(" step "); Serial.println(stepLine);
	Serial.println();
#endif
#ifdef STRIP_ENABLED
	Serial.print("Strip : anim "); Serial.print(effectStrip);
	Serial.print(" step "); Serial.println(stepStrip);
	Serial.println();
#endif
}
#endif

#ifdef LINE_ENABLED
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

void forceLine(short forced) {
	delayLine = stepLine = 0;
	forcedLine = forced;
	if(!lineDisabled) {
		handleLine();
	}
}
#endif

#ifdef STRIP_ENABLED
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

void forceStrip(short forced) {
	delayStrip = stepStrip = 0;
	forcedStrip = forced;
	if (!stripDisabled) {
		handleStrip();
	}
}
#endif

void handleButtons(byte newButtons) {
#ifndef ARDUINO_attiny
	Serial.print("handle buttons "); Serial.println(newButtons);
#endif
#ifdef LINE_ENABLED
	if ((buttons & BUTTON_LINE) != (newButtons & BUTTON_LINE)) {
		lineDisabled = newButtons & BUTTON_LINE;
		if (lineDisabled) {
			lineOff();
		}
	}
#endif
#ifdef STRIP_ENABLED
	if ((buttons & BUTTON_STRIP) != (newButtons & BUTTON_STRIP)) {
		stripDisabled = newButtons & BUTTON_STRIP;
		if (stripDisabled) {
			stripOff();
		}
	}
#endif
	if ((buttons & BUTTON_TEST) != (newButtons & BUTTON_TEST)) {
		testMode = newButtons & BUTTON_TEST;
		if (testMode) {
#ifdef LINE_ENABLED
			forceLine(0);
#endif
#ifdef STRIP_ENABLED
			forceStrip(0);
#endif
		} else {
#ifdef LINE_ENABLED
			forceLine(-1);
#endif
#ifdef STRIP_ENABLED
			forceStrip(-1);
#endif
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
#ifdef LINE_ENABLED
			lineDisabled = true;
			lineOff();
#endif
#ifdef STRIP_ENABLED
			stripDisabled = true;
			stripOff();
#endif
		} else if (b == '+') {
#ifdef LINE_ENABLED
			lineDisabled = false;
			stepLine = effectLine = 0;
#endif
#ifdef STRIP_ENABLED
			stripDisabled = false;
			effectStrip = stepStrip = 0;
#endif
#ifdef LINE_ENABLED
		} else if (b >= '0' && b <= '9') {
			forceLine((b - '0') % nbLineEffects);
#endif
#ifdef STRIP_ENABLED
		} else if (b >= 'A' && b <= 'Z') {
			forceStrip((b - 'A') % nbEffectsStrip);
#endif
		} else if (b == '.') {
#ifdef LINE_ENABLED
			forcedLine = -1;
#endif
#ifdef STRIP_ENABLED
			forcedStrip = -1;
#endif
		} else if (b == '?') {
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
#ifdef LINE_ENABLED
	if (!lineDisabled) {
		handleLine();
	}
#endif
#ifdef STRIP_ENABLED
	if (!stripDisabled) {
		handleStrip();
	}
#endif

	delay(1);
}
