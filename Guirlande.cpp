#include "Guirlande.h"
#include "Animation.h"

#include "EffectOnOff.cpp"
#include "EffectRomain.cpp"
#include "EffectAll.cpp"
#include "EffectStack.cpp"
#include "EffectPeaks.cpp"
//#include "EffectShade.cpp"
#include "EffectScroll.cpp"

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
	digitalWrite(lineGround, LOW); // led gnd => HIGH = on

	usage();
	Serial.println("ready"); Serial.flush();
}

byte line[6] = { 0, };

void lineOff() {
	for (byte i = 0; i < 6; i++) {
		digitalWrite(linePins[i], LOW);
	}
}

void lineUpdate() {
	for (byte i = 0; i < 6; i++) {
		analogWrite(linePins[i], (line[i]));
	}
}

class rotate6_1: public Animation {
	long doStep(int step) {
		if (step == 6 * 20) {
			return -1;
		}
		line[step % 6] = 0;
		line[(step+1) % 6] = 255;
		lineUpdate();

		return 50;
	}
};
class rotate6_2: public Animation {
	long doStep(int step) {
		if (step == 0) {
			line[0] = line[1] = 255;
		} else if (step == 6 * 20) {
			return -1;
		} else {
			line[(step+5) % 6] = 0;
			line[(step+1) % 6] = 255;
		}
		lineUpdate();

		return 50;
	}
};

class fade6_3: public Animation {
	long doStep(int step) {
		if (step == 0) {
			line[0] = line[1] = 255;
		} else if (step == 256 * 5 * 6) {
			return -1;
		} else {
			byte light = step % 256;
			byte rank = step / 256;
			line[(rank+5) % 6] = ~light;
			line[(rank+1) % 6] = light;
		}
		lineUpdate();

		return 2;
	}
};

Animation* effectsA[] = {
//	nbSteps: 256 * 5 * 6,
	new fade6_3(),
//	nbSteps: 6 * 20,
	new rotate6_1(),
//	nbSteps: 6 * 20,
	new rotate6_2()
};

Animation* effectsB[] = {
	new EffectAll(),
	new EffectStack(),
	new EffectRomain(),
	new EffectPeaks(),
//	nbSteps: 1,
//	new EffectShade(),
	new EffectScroll(),
	new EffectOnOff()
};

const unsigned int nbEffectsA = sizeof(effectsA) / sizeof(Animation*);
unsigned int effectA = 0, stepA = 0;
int nextA = 0;
int forcedA = -1;

const unsigned int nbEffectsB = sizeof(effectsB) / sizeof(Animation*);
unsigned int effectB = 0, stepB = 0;
int nextB = 0;
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
			stepB = 0;
		} else if (b == '.') {
			forcedA = -1;
			forcedB = -1;
		} else if (b == '?') {
			usage();
		}
	}

	if (!off) {
		if (nextA == 0) {
			if (stepA == 0) {
				lineOff();
			}
			nextA = effectsA[effectA]->doStep(stepA);
			if (nextA == -1) {
				stepA = 0;
				if (forcedA == -1) {
					effectA++;
					if (effectA >= nbEffectsA) {
						effectA = 0;
					}
				}
				nextA = 0;
			} else {
				stepA++;
			}
		} else {
			nextA--;
		}

		if (nextB == 0) {
			if (stepB == 0) {
				stripOff();
			}
			nextB = effectsB[effectB]->doStep(stepB);
			if (nextB == -1) {
				stepB = 0;
				if (forcedB == -1) {
					effectB++;
					if (effectB >= nbEffectsB) {
						effectB = 0;
					}
				}
				nextB = 0;
			} else {
				stepB++;
			}
		} else {
			nextB--;
		}
	}
	delay(1);
}
