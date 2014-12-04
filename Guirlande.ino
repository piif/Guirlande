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

// PWM'able pins
const byte linePins[6] = { 3, 5, 6, 9, 10, 11 };
const byte lineGround = 8;

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

	for (byte i = 0; i < 6; i++) {
		pinMode(linePins[i], OUTPUT);
		digitalWrite(linePins[i], HIGH); // led gnd => HIGH = off
	}
	pinMode(lineGround, OUTPUT);
	digitalWrite(lineGround, HIGH); // led gnd => HIGH = on

	usage();
	Serial.println("ready"); Serial.flush();
}

byte line[6] = { 0, };

void lineOff() {
	for (byte i = 0; i < 6; i++) {
		digitalWrite(linePins[i], HIGH);
	}
}

void lineUpdate() {
	for (byte i = 0; i < 6; i++) {
		analogWrite(linePins[i], ~(line[i]));
	}
}

unsigned long rotate6_1(int step) {
	line[step % 6] = 0;
	line[(step+1) % 6] = 255;
    lineUpdate();

	return 50;
}
unsigned long rotate6_2(int step) {
	if (step == 0) {
		line[0] = line[1] = 255;
	} else {
		line[(step+5) % 6] = 0;
		line[(step+1) % 6] = 255;
	}
    lineUpdate();

	return 50;
}
unsigned long fade6_3(int step) {
	if (step == 0) {
		line[0] = line[1] = 255;
	} else {
		byte light = step % 256;
		byte rank = step / 256;
		line[(rank+5) % 6] = ~light;
		line[(rank+1) % 6] = light;
	}
    lineUpdate();

	return 2;
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

Effect effectsA[] = {
	{
		nbSteps: 256 * 5 * 6,
		func: fade6_3
	},
	{
		nbSteps: 6 * 20,
		func: rotate6_1
	},
	{
		nbSteps: 6 * 20,
		func: rotate6_2
	}
};

Effect effectsB[] = {
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
const unsigned int nbEffectsA = sizeof(effectsA) / sizeof(Effect);
unsigned int effectA = 0, stepA = 0, nextA = 0;
int forcedA = -1;

const unsigned int nbEffectsB = sizeof(effectsB) / sizeof(Effect);
unsigned int effectB = 0, stepB = 0, nextB = 0;
int forcedB = -1;

bool off = false;

void loop() {
	if (Serial.available()) {
		int b = Serial.read();
		if (b == '-') {
			off = true;
			stripOff();
			digitalWrite(8, LOW);
		} else if (b == '+') {
			digitalWrite(8, HIGH);
			off = false;
			stepA = effectA = 0;
		} else if (b >= '0' && b <= '9') {
			forcedA = (b - '0') % nbEffectsA;
			effectA = forcedA;
			stepA = 0;
		} else if (b >= 'A' && b <= 'Z') {
			forcedB = (b - 'A') % nbEffectsB;
			effectB = forcedB;
			stepA = 0;
		} else if (b == '.') {
			forcedA = -1;
			forcedB = -1;
		} else if (b == '?') {
			usage();
		}
	}

	if (!off) {
		if (nextA == 0) {
			if (stepA >= effectsA[effectA].nbSteps) {
				stepA = 0;
				if (forcedA == -1) {
					effectA++;
					if (effectA >= nbEffectsA) {
						effectA = 0;
					}
				}
			}
			if (stepA == 0) {
				lineOff();
			}
			nextA = effectsA[effectA].func(stepA);
			stepA++;
		}
		nextA--;

		if (nextB == 0) {
			if (stepB >= effectsB[effectB].nbSteps) {
				stepB = 0;
				if (forcedB == -1) {
					effectB++;
					if (effectB >= nbEffectsB) {
						effectB = 0;
					}
				}
			}
			if (stepB == 0) {
				stripOff();
			}
			nextB = effectsB[effectB].func(stepB);
			stepB++;
		}
		nextB--;
	}
	delay(1);
}
