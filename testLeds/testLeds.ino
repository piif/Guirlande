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

#define WITH_NUMS
// define constant above to test mapping between LED number list and outputs

#undef WITH_BUTTONS
// define constant above to test mapping with button inputs instead of serial (to use with ATtiny)

#undef WITH_ANIM
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

// deduce other constants from previous defines

#ifdef WITH_BUTTONS
	#include "buttons.h"
#endif

#if defined(WITH_ANIM) || defined(WITH_NUMS)
	#include "setInterval.h"
#endif

#if defined(ARDUINO_AVR_UNO)

	#define PIN_A 8
	#define PIN_B 9
	#define PIN_C 10

	// masks for DDRx and PORTx
	// DDRB = 0 for output , 1 for INPUT
	// PORTB = output
	// thus :
	//  DDRB=1 , PORTB=1 => +5
	//  DDRB=1 , PORTB=0 => GND
	//  DDRB=0 , PORTB=? => Z

	#define OUT_A 0x01
	#define OUT_B 0x02
	#define OUT_C 0x04

	#define PORT PORTB
	#define DDR DDRB
	#ifdef WITH_PWM
//		#define OCR_OUT_A OCR1A
		#define OCR_OUT_B OCR1A
		#define OCR_OUT_C OCR1B
	#endif

#elif defined(ARDUINO_attiny)

	#define PIN_A 0 // pin 5
	#define PIN_B 1 // pin 6
	#define PIN_C 4 // pin 3

	// masks for DDRx and PORTx
	// DDRB = 0 for output , 1 for INPUT
	// PORTB = output
	// thus :
	//  DDRB=1 , PORTB=1 => +5
	//  DDRB=1 , PORTB=0 => GND
	//  DDRB=0 , PORTB=? => Z

	#define OUT_A 0x01
	#define OUT_B 0x02
	#define OUT_C 0x10

	#define PORT PORTB
	#define DDR DDRB
	#ifdef WITH_PWM
//		#define OCR_OUT_A OCR0A
		#define OCR_OUT_B OCR1A
		#define OCR_OUT_C OCR1B
	#endif

#else
	#error device unsupported
#endif

#define OUT_NOT (~(OUT_A | OUT_B | OUT_C))

#ifdef WITH_PWM
	#define MAX_INTENSITY 255
#endif


#ifdef WITH_MASK

#define MAX_BUFFER 4
char buffer[MAX_BUFFER];
int index = 0;

void parseBit(char bit, byte output) {
	switch(bit) {
	case '0':
	case '-':
		pinMode(output, OUTPUT);
#ifdef WITH_PWM
		switch(output) {
		case PIN_A:
			OCR_OUT_A = 128;
			break;
		case PIN_B:
			OCR_OUT_B = 128;
			break;
		case PIN_C:
			OCR_OUT_C = 128;
			break;
		}
#else
		digitalWrite(output, 0);
#endif
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
#ifdef WITH_PWM
	OCR_OUT_A = 0;
	OCR_OUT_B = 0;
	OCR_OUT_C = 0;
#endif
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

// masks for DDR and PORT
// DDR = 0 for output , 1 for INPUT
// PORT = output
// thus :
//  DDR=1 , PORT=1 => +5
//  DDR=1 , PORT=0 => GND
//  DDR=0 , PORT=? => Z

byte leds[6] = { 0, };
setIntervalTimer displayTimer = 0;

#ifdef BY_6
void displayStepCallback_6(void *userData, long late, int missed) {
	byte port = PORT & OUT_NOT, ddr = DDR & OUT_NOT; // clear bits => set to Z
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
		// A = 0;
		ddr |= OUT_A;

		// C = leds[0]>0 ? leds[0] : 'z';
		if (leds[0] > 0) {
			ddr |= OUT_C;
#ifdef WITH_PWM
			OCR_OUT_C = leds[0];
#endif
		}

		displaySubStep = 1;
	break;
	case 1:
		// A = 1
		port |= OUT_A;
		ddr |= OUT_A;

		// C = leds[1]>0 ? 256-leds[1] : 'z';
		if (leds[1] > 0) {
			ddr |= OUT_C;
#ifdef WITH_PWM
			OCR_OUT_C = ~leds[1];
#endif
		}

		displaySubStep = 2;
	break;
	case 2:
		// A = 1;
		port |= OUT_A;
		ddr |= OUT_A;

		// B = leds[2]>0 ? 256-leds[2] : 'z';
		if (leds[2] > 0) {
			ddr |= OUT_B;
#ifdef WITH_PWM
			OCR_OUT_B = ~leds[2];
#endif
		}

		displaySubStep = 3;
	break;
	case 3:
		// A = 0;
		ddr |= OUT_A;

		// B = leds[3] ? leds[3] : 'z';
		if (leds[3] > 0) {
			ddr |= OUT_B;
#ifdef WITH_PWM
			OCR_OUT_B = leds[3];
#endif
		} // else ddr already cleared

		displaySubStep = 4;
	break;
	case 4:
		// C = '1';
		ddr |= OUT_C;
#ifdef WITH_PWM
		port |= OUT_C;
		OCR_OUT_C = 0x0;
#endif

		// B = leds[4]>0 ? leds[4] : 'z';
		if (leds[4] > 0) {
			ddr |= OUT_B;
#ifdef WITH_PWM
			OCR_OUT_B = leds[4];
#endif
		}

		displaySubStep = 5;
	break;
	case 5:
		// B = 0;
		ddr |= OUT_B;
#ifdef WITH_PWM
		port |= OUT_B;
		OCR_OUT_B = 0x0;
#endif

		// C = leds[5]>0 ? leds[5] : 'z';
		if (leds[5] > 0) {
			ddr |= OUT_C;
#ifdef WITH_PWM
			OCR_OUT_C = leds[5];
#endif
		}

		displaySubStep = 0;
	break;
	}
	PORT = port;
	DDR  = ddr;
}
#endif // BY_6

#ifdef BY_3
void displayStepCallback_3(void *userData, long late, int missed) {
	byte port = PORT & OUT_NOT, ddr = DDR & OUT_NOT; // clear bits 3, 4 & 5 => Z
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
		port |= OUT_A;
		ddr |= OUT_A;
#ifdef WITH_PWM
		OCR_OUT_A = 0;
#endif

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
		port |= OUT_B;
		ddr |= OUT_B;
#ifdef WITH_PWM
		OCR_OUT_B = 0;
#endif

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
		port |= OUT_C;
		ddr |= OUT_C;
#ifdef WITH_PWM
		OCR_OUT_C = 0;
#endif

		displaySubStep = 0;
	break;
	}
	PORT = port;
	DDR  = ddr;

//	Serial.print("PORT "); Serial.println(port, BIN);
//	Serial.print("DDR  "); Serial.println(ddr, BIN);
}
#endif // BY_3

#endif // defined(WITH_NUMS) || defined(WITH_ANIM)

#ifdef WITH_NUMS

#ifdef WITH_BUTTONS
	byte buttons = 0;
#else
	#define MAX_BUFFER 25
	char buffer[MAX_BUFFER];
	int index = 0;
#endif

#ifndef WITH_BUTTONS
byte intensity = 128;

void parseNums(char *buffer) {
	Serial.print("Parsing "); Serial.println(buffer);
	if (buffer[0] == ':') {
		char *c = buffer+1;
		char l=0;
		leds[l] = 0;
		while(*c != '\0') {
			if (*c == ' ') {
				l++;
				leds[l] = 0;
			} else {
				leds[l] = leds[l] * 10 + (*c - '0');
			}
			c++;
		}
		return;
	}
	if (buffer[0] == '+') {
		char *c = buffer+1;
		intensity=0;
		while(*c != '\0') {
			intensity = intensity * 10 + (*c - '0');
			c++;
		}
		Serial.print("Intensity = "); Serial.println(intensity);
		for(byte l = 0; l < 6; l++) {
			if (leds[l] != 0) {
				leds[l] = intensity;
			}
		}
		return;
	}
	for(byte l = 0; l < 6; l++) {
		leds[l] = 0;
	}
	for(char *c = buffer; *c; c++) {
		if (*c >= '1' && *c <= '6') {
			leds[*c-'1'] = intensity;
		}
	}
}
#endif

void readNums() {
#ifdef WITH_BUTTONS
	byte newButtons = readButtons();
	if (newButtons == 255 || newButtons == buttons) {
		return;
	}

#ifndef ARDUINO_attiny
	Serial.println(analogRead(BUTTON_INPUT));
	Serial.println(newButtons);
#endif
	if (buttons==7) {
		leds[0] = leds[1] = leds[2] = leds[3] = leds[4] = leds[5] = 0;
	} else if (buttons>0) {
		leds[buttons-1] = 0;
	}
	if (newButtons==7) {
		leds[0] = leds[1] = leds[2] = leds[3] = leds[4] = leds[5] = 210;
	} else if (newButtons>0) {
		leds[newButtons-1] = 50;
	}
	buttons = newButtons;

//	int value = analogRead(BUTTON_INPUT);
//	for(int i=5, mask = 64 ; i>=0; i--, mask >>= 1) {
//		leds[i] = (value & mask) != 0;
//	}

#else
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
#endif // WITH_BUTTONS
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
#ifndef ARDUINO_attiny
	Serial.begin(115200);
#endif
#if defined(WITH_MASK)
	pinMode(PIN_A, INPUT);
	pinMode(PIN_B, INPUT);
	pinMode(PIN_C, INPUT);
#endif

#ifdef WITH_PWM

#if defined(ARDUINO_AVR_UNO)
	// COM1A = COM1B = 2 = clear on compare match
	// WGM = 5 = Fast PWM, 8-bit , TOP=0xFF
	// CS = 1 = no prescaling
	// FOC1A = FOC1B = Force output compare
	TCCR1A = 0xA1; TCCR1B = 0x09; TCCR1C = 0xC0;
#elif defined(ARDUINO_attiny)
	// CTC = 0 = no counter reset on compare match
	// COM1A = COM1B = 2 = clear on compare match
	// PWM1A = PWM1B = 1 = PWM, TOP=OCR1C=0xFF
	// CS = 1 = no prescaling
	// FOC1A = FOC1B = Force output compare
	TCCR1 = 0x61; GTCCR = 0x6C;
	OCR1C = 0xFF;
#else
#error device unsupported
#endif

#endif // WITH_PWM

#ifdef WITH_ANIM
	animationTimer = setInterval(500, animationStepCallback, 0);
	startAnim(-1);
#endif

#if defined(WITH_NUMS) || defined(WITH_ANIM)
#ifdef BY_6
	displayTimer = setInterval(1, displayStepCallback_6, 0);
#else
	displayTimer = setInterval(5, displayStepCallback_3, 0);
#endif
#endif

#ifndef ARDUINO_attiny
	Serial.println("ready");
#endif
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
	delay(1);
}
