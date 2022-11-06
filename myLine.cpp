#include "myLine.h"

#define OUT_NOT (~(OUT_A | OUT_B | OUT_C))

byte leds[6] = { 0, };

void lineOff() {
	PORT &= OUT_NOT;
	DDR &= OUT_NOT;
}

void lineSet(byte leds[], byte value) {
	leds[0] = value;
	leds[1] = value;
	leds[2] = value;
	leds[3] = value;
	leds[4] = value;
	leds[5] = value;
}

void lineSet(byte value) {
	leds[0] = value;
	leds[1] = value;
	leds[2] = value;
	leds[3] = value;
	leds[4] = value;
	leds[5] = value;
}

void lineInit() {
	pinMode(PIN_A, INPUT);
	pinMode(PIN_B, INPUT);
	pinMode(PIN_C, INPUT);

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
}

#ifdef BY_6
void displayStep() {
	byte port = PORT & OUT_NOT, ddr = DDR & OUT_NOT; // clear bits 3, 4 & 5 => Z
	static byte displaySubStep = 0;

	//	10z -> 1
	//	z10 -> 2
	//	1z0 -> 3
	//	01z -> 4
	//	z01 -> 5
	//	0z1 -> 6

	//	100 -> 1+3
	//	010 -> 2+4
	//	001 -> 5+6

	//	110 -> 2+3
	//	011 -> 4+6
	//	101 -> 1+5

	switch(displaySubStep) {
	case 0: // 1 !x z
		// A = 1;
		port |= OUT_A;
		ddr |= OUT_A;

		// B = leds[2]>0 ? 256-leds[2] : 'z';
		if (leds[2] > 0) {
			ddr |= OUT_B;
			OCR_OUT_B = ~leds[2];
		}
	break;
	case 1: // z x 0
		// C = 0;
		ddr |= OUT_C;
		port |= OUT_C;
		OCR_OUT_C = 0x0;

		// B = leds[4]>0 ? leds[4] : 'z';
		if (leds[4] > 0) {
			ddr |= OUT_B;
			OCR_OUT_B = leds[4];
		}
	break;
	case 2: // 1 z !x
		// A = 1
		port |= OUT_A;
		ddr |= OUT_A;

		// C = leds[1]>0 ? 256-leds[1] : 'z';
		if (leds[1] > 0) {
			ddr |= OUT_C;
			OCR_OUT_C = ~leds[1];
		}
	break;
	case 3: // 0 x z
		// A = 0;
		ddr |= OUT_A;

		// B = leds[3] ? leds[3] : 'z';
		if (leds[3] > 0) {
			ddr |= OUT_B;
			OCR_OUT_B = leds[3];
		} // else ddr already cleared
	break;
	case 4: // z 0 x
		// B = 0;
		ddr |= OUT_B;
		port |= OUT_B;
		OCR_OUT_B = 0x0;

		// C = leds[5]>0 ? leds[5] : 'z';
		if (leds[5] > 0) {
			ddr |= OUT_C;
			OCR_OUT_C = leds[5];
		}
	break;
	case 5: // 0 z x
		// A = 0;
		ddr |= OUT_A;

		// C = leds[0]>0 ? leds[0] : 'z';
		if (leds[0] > 0) {
			ddr |= OUT_C;
			OCR_OUT_C = leds[0];
		}
	break;
	}
	PORT = port;
	DDR  = ddr;
	displaySubStep = (displaySubStep + 1) % 6;
}

#else // BY_6

void displayStep() {
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
		port |= OUT_A; OCR_OUT_A = 0; ddr |= OUT_A;

		// B = leds[2] ? '0' : 'z';
		if (leds[2] > 0) {
			ddr |= OUT_B;
			OCR_OUT_B = leds[2];
		} // else ddrd already cleared

		// C = leds[1] ? '0' : 'z';
		if (leds[1] > 0) {
			ddr |= OUT_C;
			OCR_OUT_C = leds[1];
		}
		displaySubStep = 1;
	break;
	case 1:
		// A = leds[3] ? '0' : 'z';
		if (leds[3] > 0) {
			ddr |= OUT_A;
			OCR_OUT_A = leds[3];
		}

		// B = '1';
		port |= OUT_B; OCR_OUT_B = 0; ddr |= OUT_B;

		// C = leds[4] ? '0' : 'z';
		if (leds[4] > 0) {
			ddr |= OUT_C;
			OCR_OUT_C = leds[4];
		}
		displaySubStep = 2;
	break;
	case 2:
		// A = leds[0] ? '0' : 'z';
		if (leds[0] > 0) {
			ddr |= OUT_A;
			OCR_OUT_A = leds[0];
		}

		// B = leds[5] ? '0' : 'z';
		if (leds[5] > 0) {
			ddr |= OUT_B;
			OCR_OUT_B = leds[5];
		}

		// C = '1';
		port |= OUT_C; OCR_OUT_C = 0; ddr |= OUT_C;

		displaySubStep = 0;
	break;
	}
	PORT = port;
	DDR  = ddr;

//	Serial.print("PORT "); Serial.println(port, BIN);
//	Serial.print("DDR  "); Serial.println(ddr, BIN);
}

#endif // BY_6

