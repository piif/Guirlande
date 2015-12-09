#include "Guirlande.h"

// PWM'able pins
const byte linePins[6] = { 3, 5, 6, 9, 10, 11 };
const byte lineGround = 8;

void usage() {
	Serial.println("? => this help");
	Serial.println("+ => light on");
	Serial.println("- => light off");
	Serial.println("0..9 => force to loop on this line effect");
	Serial.println("A..Z => force to loop on this strip effect");
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
		analogWrite(linePins[i], line[i]);
	}
}

long rotate6_1(long step) {
	if (step == 6 * 20) {
		return -1;
	}
	line[step % 6] = 0;
	line[(step+1) % 6] = 255;
	lineUpdate();

	return 50;
}

long rotate6_2(long step) {
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

long fade6_3(long step) {
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

effectFunction effectsLine[] = {
	fade6_3,
	rotate6_1,
	rotate6_2
};

effectFunction effectsStrip[] = {
	EffectDrops,
	EffectAll,
	EffectStack,
	EffectRomain,
	EffectPeaks,
	EffectShade,
	EffectScroll,
	EffectOnOff
};

const unsigned int nbEffectsLine = sizeof(effectsLine) / sizeof(effectFunction);
int effectLine = 0, forcedLine = -1;
unsigned long stepLine = 0;
long delayLine = 0;

const unsigned int nbEffectsStrip = sizeof(effectsStrip) / sizeof(effectFunction);
int effectStrip = 0, forcedStrip = -1;
unsigned long stepStrip = 0;
long delayStrip = 0;

bool off = false;

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
			stepLine = effectLine = 0;
			effectStrip = stepStrip = 0;
		} else if (b >= '0' && b <= '9') {
			forcedLine = (b - '0') % nbEffectsLine;
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

	if (!off) {
		if (delayLine == 0) {
			if (stepLine == 0) {
				lineOff();
			}
			delayLine = effectsLine[effectLine](stepLine);
			if (delayLine == -1) {
				stepLine = 0;
				if (forcedLine == -1) {
					effectLine++;
					if (effectLine >= nbEffectsLine) {
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
	delay(1);
}
