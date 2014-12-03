#ifdef PIF_TOOL_CHAIN
	#include <Arduino.h>
	// other includes with full pathes
	// example : #include "led7/led7.h"
	#include <ledStrip/strip.h>
#else
	// other includes with short pathes
	// example : #include "led7.h"
	#include <strip.h>
#endif

#ifndef DEFAULT_BAUDRATE
	#define DEFAULT_BAUDRATE 115200
#endif

#define STRIP_PIN A0
#define STRIP_LEN 60

void usage() {
	Serial.println("? => this help");
	Serial.println("+ => light on");
	Serial.println("- => light off");
	Serial.println("0..9 => force to loop on this effect");
	Serial.println(". => iterate on effects");
	Serial.flush();
}

void setup(void) {
	Serial.begin(DEFAULT_BAUDRATE);

	stripInit(STRIP_LEN, STRIP_PIN);
	stripUpdate(); // Initialize all pixels to 'off'

	usage();
	Serial.println("ready"); Serial.flush();
}

unsigned long effectOnOff(int step) {
	static bool flag = HIGH;
	static byte h = 0;
	static byte position = 0;
	if (flag) {
		stripSetH(position, h);
		h = (h + 3) % 256;
	} else {
		stripSetH(position, 0);
	}
    stripUpdate();
    position++;
	if (position == STRIP_LEN) {
		position = 0;
		flag = flag ? LOW : HIGH;
	}
	return 5;
}

unsigned long effectScroll(int step) {
	if (step == 0) {
		for(byte i = 0; i < STRIP_LEN; i++) {
			stripSetH(i, (i+1) * 4);
		}
	} else {
		byte first = stripGetH(0);
		for (int i = 1; i < STRIP_LEN; i++) {
			stripSetH(i - 1, stripGetH(i));
		}
		stripSetH(STRIP_LEN - 1, first);
		stripUpdate();
	}

	return 25;
}

unsigned long effectPeaks(int step) {
	static int peak = STRIP_LEN / 7;
	static byte h = 3;

	for (int i = peak - 9; i < peak + 9; i++) {
		if (i < 0 || i > STRIP_LEN) {
			continue;
		}
		byte level = 9 - abs(9 - (step % 19));
		byte dist = abs(peak - i);
		if (level == 0) {
			stripSetH(i, 0);
		} else {
			stripSetHL(i, h, (10 - dist) * level * 2);
		}
	}
	if (step % 19 == 18) {
		peak = (peak + 17) % STRIP_LEN;
		h = (h + 13) % 255 + 1;
	}
    stripUpdate();
	return 25;
}

unsigned long effectShade(int step) {
	static byte h = 17;

	for(byte i = 0; i < STRIP_LEN; i++) {
		stripSetHL(i, h, (i+1) * 3);
	}

    stripUpdate();
	h = h * 4;
	return 25;
}

unsigned long effectAll(int step) {
	static Color colors[] = {
		{g:0,r:255,b:0},
		{g:255,r:0,b:0},
		{g:0,r:0,b:255},
		{g:255,r:255,b:255}
	};
	stripAll(colors[step]);
	return 500;
}

typedef unsigned long (*EffectCallback)(int);

typedef struct _effect {
	unsigned int nbSteps;
	EffectCallback func;
} Effect;

Effect effects[] = {
	{
		nbSteps: 4,
		func: effectAll
	},
	{
		nbSteps: 19 * 10,
		func: effectPeaks
	},
	{
		nbSteps: 1,
		func: effectShade
	},
	{
		nbSteps: 5 * 60,
		func: effectScroll
	},
	{
		nbSteps: 9 * 60,
		func: effectOnOff
	}
};
const unsigned int nbEffects = sizeof(effects) / sizeof(Effect);

unsigned int effect = 0, step = 0, next = 0;
int forced = -1;
bool off = false;

void loop() {
	if (Serial.available()) {
		int b = Serial.read();
		if (b == '-') {
			off = true;
			stripOff();
		} else if (b == '+') {
			off = false;
			step = effect = 0;
		} else if (b >= '0' && b <= '9') {
			forced = (b - '0') % nbEffects;
			effect = forced;
			step = 0;
		} else if (b == '.') {
			forced = -1;
		} else if (b == '?') {
			usage();
		}
	}

	if (!off) {
		if (next == 0) {
			if (step >= effects[effect].nbSteps) {
				step = 0;
				if (forced == -1) {
					effect++;
					if (effect >= nbEffects) {
						effect = 0;
					}
				}
			}
			if (step == 0) {
				stripOff();
			}
			next = effects[effect].func(step);
			step++;
		}
		next--;
	}
	delay(1);
}
