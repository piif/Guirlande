/**
 * test code to validate wiring and result in terms of light intensity and flickering
 */
#include <Arduino.h>

#undef WITH_MASK
// define constant above to test basic wiring :
// user inputs 3 chars values, each char being 1 (or +), 0 (or -) or z (or anything else)
//	1 => set output to +5
//	0 => set output to GND
//	z => set output to Z (input)

// Masks values:
//	0z1 -> 1
//	1z0 -> 2
//	10z -> 3
//	01z -> 4
//	z10 -> 5
//	z01 -> 6
//	011 -> 1+4
//	110 -> 2+5
//	101 -> 3+6
//	100 -> 2+3
//	010 -> 4+5
//	001 -> 1+6
//	other => nothing

#undef WITH_NUMS
// define constant above to test mapping between LED number list and outputs

#define WITH_ANIM
// define constant above to test animations : user inputs a letter to launch related animation or '-' to stop.

// following constants are used for WITH_NUMS and WITH_ANIM modes
#undef BY_3
// define constant above to test display in 3 steps
// should be more efficient, but intensity may vary depending on which LEDs are on or off
#define BY_6
// define constant above to test display in 6 steps
// user inputs up to 6 values between 1 and 6
// listed LED should be set "on"

// undef to use on/off mode , def to use variable intensity thru PWM
#define WITH_PWM

#define MAX_INTENSITY 255

#if defined(WITH_ANIM) || defined(WITH_NUMS)
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

#ifdef WITH_MASK

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
#endif

#if defined(WITH_NUMS) || defined(WITH_ANIM)

// masks for DDRB and PORTB
// DDRB = 0 for output , 1 for INPUT
// PORTB = output
// thus :
//  DDRB=1 , PORTB=1 => +5
//  DDRB=1 , PORTB=0 => GND
//  DDRB=0 , PORTB=? => Z

#define OUT_A 0x02
#define OUT_B 0x04
#define OUT_C 0x08

#define OCR_OUT_A OCR1A
#define OCR_OUT_B OCR1B
#define OCR_OUT_C OCR2A

#define OUT_NOT (~(OUT_A | OUT_B | OUT_C))

#define MAX_BUFFER 8
char buffer[MAX_BUFFER];
int index = 0;

byte leds[6] = { 0, };
setIntervalTimer displayTimer = 0;

#ifdef BY_6
void displayStepCallback_6(void *userData, long late, int missed) {
	byte port = PORTB & OUT_NOT, ddr = DDRB & OUT_NOT; // clear bits 3, 4 & 5 => Z
	static byte displaySubStep= 0;

	//	10z -> 3
	//	1z0 -> 2
	//	100 -> 2+3

	//	01z -> 4
	//	z10 -> 5
	//	010 -> 4+5

	//	0z1 -> 1
	//	z01 -> 6
	//	001 -> 1+6

	switch(displaySubStep) {
	case 0:
		// A = '1';
		port |= OUT_A; OCR_OUT_A = 0; ddr |= OUT_A;

		// B = leds[2] ? '0' : 'z';
		if (leds[2] > 0) {
			ddr |= OUT_B;
#ifdef WITH_PWM
			OCR_OUT_B = leds[2];
#endif
		} // else ddrd already cleared

		displaySubStep = 1;
	break;
	case 1:
		// A = '1';
		port |= OUT_A; OCR_OUT_A = 0; ddr |= OUT_A;

		// C = leds[1] ? '0' : 'z';
		if (leds[1] > 0) {
			ddr |= OUT_C;
#ifdef WITH_PWM
			OCR_OUT_C = leds[1];
#endif
		}
		displaySubStep = 2;
	break;
	case 2:
		// B = '1';
		port |= OUT_B; OCR_OUT_B = 0; ddr |= OUT_B;

		// A = leds[3] ? '0' : 'z';
		if (leds[3] > 0) {
			ddr |= OUT_A;
#ifdef WITH_PWM
			OCR_OUT_A = leds[3];
#endif
		}

		displaySubStep = 3;
	break;
	case 3:
		// B = '1';
		port |= OUT_B; OCR_OUT_B = 0; ddr |= OUT_B;

		// C = leds[4] ? '0' : 'z';
		if (leds[4] > 0) {
			ddr |= OUT_C;
#ifdef WITH_PWM
			OCR_OUT_C = leds[4];
#endif
		}
		displaySubStep = 4;
	break;
	case 4:
		// C = '1';
		port |= OUT_C; OCR_OUT_C = 0; ddr |= OUT_C;

		// A = leds[0] ? '0' : 'z';
		if (leds[0] > 0) {
			ddr |= OUT_A;
#ifdef WITH_PWM
			OCR_OUT_A = leds[0];
#endif
		}

		displaySubStep = 5;
	break;
	case 5:
		// C = '1';
		port |= OUT_C; OCR_OUT_C = 0; ddr |= OUT_C;

		// B = leds[5] ? '0' : 'z';
		if (leds[5] > 0) {
			ddr |= OUT_B;
#ifdef WITH_PWM
			OCR_OUT_B = leds[5];
#endif
		}

		displaySubStep = 0;
	break;
	}
	PORTB = port;
	DDRB  = ddr;
}
#endif // BY_6

#ifdef BY_3
void displayStepCallback_3(void *userData, long late, int missed) {
	byte port = PORTB & OUT_NOT, ddr = DDRB & OUT_NOT; // clear bits 3, 4 & 5 => Z
	static byte displaySubStep= 0;

	//	10z -> 3
	//	1z0 -> 2
	//	100 -> 2+3

	//	01z -> 4
	//	z10 -> 5
	//	010 -> 4+5

	//	0z1 -> 1
	//	z01 -> 6
	//	001 -> 1+6

	switch(displaySubStep) {
	case 0:
		// A = '1';
		port |= OUT_A; OCR_OUT_A = 0; ddr |= OUT_A;

		// B = leds[2] ? '0' : 'z';
		if (leds[2] > 0) {
			ddr |= OUT_B;
#ifdef WITH_PWM
			OCR_OUT_B = leds[2];
#endif
		} // else ddrd already cleared

		// C = leds[1] ? '0' : 'z';
		if (leds[1] > 0) {
			ddr |= OUT_C;
#ifdef WITH_PWM
			OCR_OUT_C = leds[1];
#endif
		}
		displaySubStep = 1;
	break;
	case 1:
		// A = leds[3] ? '0' : 'z';
		if (leds[3] > 0) {
			ddr |= OUT_A;
#ifdef WITH_PWM
			OCR_OUT_A = leds[3];
#endif
		}

		// B = '1';
		port |= OUT_B; OCR_OUT_B = 0; ddr |= OUT_B;

		// C = leds[4] ? '0' : 'z';
		if (leds[4] > 0) {
			ddr |= OUT_C;
#ifdef WITH_PWM
			OCR_OUT_C = leds[4];
#endif
		}
		displaySubStep = 2;
	break;
	case 2:
		// A = leds[0] ? '0' : 'z';
		if (leds[0] > 0) {
			ddr |= OUT_A;
#ifdef WITH_PWM
			OCR_OUT_A = leds[0];
#endif
		}

		// B = leds[5] ? '0' : 'z';
		if (leds[5] > 0) {
			ddr |= OUT_B;
#ifdef WITH_PWM
			OCR_OUT_B = leds[5];
#endif
		}

		// C = '1';
		port |= OUT_C; OCR_OUT_C = 0; ddr |= OUT_C;

		displaySubStep = 0;
	break;
	}
	PORTB = port;
	DDRB  = ddr;

//	Serial.print("PORTB "); Serial.println(port, BIN);
//	Serial.print("DDRB  "); Serial.println(ddr, BIN);
}
#endif // BY_3

#endif // defined(WITH_NUMS) || defined(WITH_ANIM)

#ifdef WITH_NUMS
void parseNums(char *buffer) {
	Serial.print("Parsing "); Serial.println(buffer);
	for(byte l = 0; l < 6; l++) {
		leds[l] = 0;
	}
	for(char *c = buffer; *c; c++) {
		if (*c >= '1' && *c <= '6') {
			leds[*c-'1'] = 1;
		}
	}
}

void readNums() {
	if (!Serial.available()) {
		return;
	}
	buffer[index] = Serial.read();
	if (buffer[index] == '\n' || buffer[index] == '\r') {
		buffer[index] = '\0';
		index=0;
		parseNums(buffer);
		return;
	}
	if (index == MAX_BUFFER - 2) {
		buffer[index + 1] = '\0';
		index = 0;
		parseNums(buffer);
	} else {
		index++;
	}
}
#endif // WITH_NUMS


#ifdef WITH_ANIM

setIntervalTimer animationTimer = 0;

typedef struct animation {
	bool (*callback)(long, byte leds[]);
	long delay;
	long nbSteps;
} Animation;

void rotateRight() {
	byte tmp = leds[5];
	leds[5] = leds[4];
	leds[4] = leds[3];
	leds[3] = leds[2];
	leds[2] = leds[1];
	leds[1] = leds[0];
	leds[0] = tmp;
}

bool aStep(long step, byte leds[]) {
	static byte led = 0;

	if (step == 0) {
		leds[0] = MAX_INTENSITY;
		leds[1] = MAX_INTENSITY >> 3;
		leds[2] = 0;
		leds[3] = 0;
		leds[4] = 0;
		leds[5] = 0;
		return true;
	}
	rotateRight();

//	leds[led] = 0;
//	if (led == 5) {
//		led = 0;
//	} else {
//		led++;
//	}
//	leds[led] = MAX_INTENSITY;

	return true;
}

bool bStep(long step, byte leds[]) {
	static byte led = 0;
	static byte on = MAX_INTENSITY;

	if (step == 0) {
		leds[0] = 0;
		leds[1] = 0;
		leds[2] = 0;
		leds[3] = 0;
		leds[4] = 0;
		leds[5] = 0;
		return true;
	}

	leds[led] = on;

	if (led == 5) {
		led = 0;
		on = (on == MAX_INTENSITY) ? 0 : MAX_INTENSITY;
	} else {
		led++;
	}
	return true;
}

bool cStep(long step, byte leds[]) {
	if (step == 0) {
		leds[0] = 0;
		leds[1] = 0;
		leds[2] = 0;
		leds[3] = 0;
		leds[4] = 0;
		leds[5] = 0;
		return true;
	}

	leds[0]++;
	leds[1] = leds[0];
	leds[2] = leds[0];
	leds[3] = leds[0];
	leds[4] = leds[0];
	leds[5] = leds[0];

//	if (leds[0] == 0) {
//		Serial.println(millis());
//	}
	return true;
}

bool dStep(long step, byte leds[]) {
	if (step == 0) {
		leds[0] = 0;
		leds[1] = MAX_INTENSITY >> 4;
		leds[2] = MAX_INTENSITY >> 2;
		leds[3] = MAX_INTENSITY;
		leds[4] = MAX_INTENSITY >> 2;
		leds[5] = MAX_INTENSITY >> 4;
	} else {
		rotateRight();
	}
	return true;
}

bool eStep(long step, byte leds[]) {
	if (step == 0) {
		leds[0] = 0;
		leds[1] = 40;
		leds[2] = 80;
		leds[3] = 120;
		leds[4] = 160;
		leds[5] = 200;
		return true;
	}

	leds[0]++;
	leds[1]++;
	leds[2]++;
	leds[3]++;
	leds[4]++;
	leds[5]++;

	return true;
}

Animation animations[] = {
	{ aStep, 250,   6 },
	{ bStep, 100,  12 },
	{ cStep,   5, 256 },
	{ dStep, 100,   6 },
	{ eStep,   5, 256 }
};
int currentAnim = -1;
#define NB_ANIMATIONS (sizeof(animations) / sizeof(Animation))

long animationStep = 0;

void startAnim(int index) {
	if (index == -1) {
		currentAnim = -1;
		Serial.println("Stop anim");
		changeInterval(animationTimer, SET_INTERVAL_PAUSED);
		return;
	}
	currentAnim = index % NB_ANIMATIONS;
	Serial.print("Start anim "); Serial.println(currentAnim);
	animationStep = 0;
	animationTimer = changeInterval(animationTimer, animations[currentAnim].delay);
}


void animationStepCallback(void *userData, long late, int missed) {
	if (currentAnim >= 0) {
		(animations[currentAnim].callback)(animationStep, leds);
		animationStep++;
	}
}

void readAnim() {
	if (!Serial.available()) {
		return;
	}
	char c = Serial.read();
	if (c == '\n' || c == '\r') {
		return;
	}
	if (c == '-') {
		startAnim(-1);
	}
	startAnim((c-'a') % NB_ANIMATIONS);
}

#endif // WITH_ANIM

void setup() {
	Serial.begin(115200);
	pinMode(PIN_A, INPUT);
	pinMode(PIN_B, INPUT);
	pinMode(PIN_C, INPUT);

#ifdef WITH_ANIM
	animationTimer = setInterval(500, animationStepCallback, 0);
	startAnim(-1);
#endif

#if defined(WITH_NUMS) || defined(WITH_ANIM)
#ifdef WITH_PWM
	TCCR1A = 0xF1; TCCR1B = 0x09; TCCR1C = 0xC0;
	TCCR2A = 0xC3; TCCR2B = 0xC1;
//	OCR_OUT_A = 50;
//	OCR_OUT_B = 128;
//	OCR_OUT_C = 200;
//	DDRB |= OUT_A | OUT_B | OUT_C;
#endif

#ifdef BY_6
	displayTimer = setInterval(1, displayStepCallback_6, 0);
#else
	displayTimer = setInterval(5, displayStepCallback_3, 0);
#endif
#endif

	Serial.println("ready");
}

void loop() {
#ifdef WITH_MASK
	readMask();
#endif
#ifdef WITH_NUMS
	readNums();
	setIntervalStep();
#endif
#ifdef WITH_ANIM
	readAnim();
	setIntervalStep();
#endif
}
