#include <Arduino.h>

#undef WITH_ANIM

#ifdef WITH_ANIM
#include "setInterval.h"
#endif

#define PIN_A 9
#define PIN_B 10
#define PIN_C 11

// setInterval(callback)
// callBack => case 1/2/3 , set A,B,C + other
// use a global 6bits mask
// main or another setInterval to play changes
// test speed limits ...

#define MAX_BUFFER 4
char buffer[MAX_BUFFER];
int index = 0;

void parseBit(char bit, byte output) {
	switch(bit) {
	case '0':
	case '-':
		pinMode(output, OUTPUT);
		digitalWrite(output, 0);
	break;
	case '1':
	case '+':
		pinMode(output, OUTPUT);
		digitalWrite(output, 1);
	break;
	default:
		pinMode(output, INPUT);
	break;
	}
}

void parseMask(char *buffer) {
	parseBit(buffer[0], PIN_A);
	parseBit(buffer[1], PIN_B);
	parseBit(buffer[2], PIN_C);
}

void readMask() {
	if (!Serial.available()) {
		return;
	}
	buffer[index] = Serial.read();
	if (buffer[index] == '\n' || buffer[index] == '\r') {
		return;
	}
	if (index == MAX_BUFFER - 2) {
		buffer[index + 1] = '\0';
		index = 0;
		parseMask(buffer);
	} else {
		index++;
	}
}

#ifdef WITH_ANIM
char *LedMaskToOutputMask[] = {
	"0z1", // 1
	"1z0", // 2
	"z10", // 3
	"z01", // 4
	"10z", // 5
	"01z"  // 6
};

setIntervalTimer animationTimer = 0;

int parseInt(char *buffer) {
	int result = 0;
	for(char *c = buffer; *c; c++) {
		result = result * 10 + *c - '0';
	}
	return result;
}

void animationStep(void *userData, long late, int missed) {
	static byte led  = 0;

	char *mask=LedMaskToOutputMask[led];
	parseMask(mask);

	led = (led + 1) % 6;
}
#endif

void setup() {
	Serial.begin(115200);
	pinMode(PIN_A, INPUT);
	pinMode(PIN_B, INPUT);
	pinMode(PIN_C, INPUT);

#ifdef WITH_ANIM
	animationTimer = setInterval(500, animationStep, 0);
#endif

	Serial.println("ready");
}

void loop() {
#ifdef WITH_ANIM
	setIntervalStep();
#else
	readMask();
#endif
}
