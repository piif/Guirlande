#include "myLine.h"

#define OUT_NOT (~(OUT_A | OUT_B | OUT_C))

byte leds[6] = { 0, };

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
	// COM1A = 3 = set on compare match
	// COM1B = 3 = set on compare match
	// WGM = 5 = Fast PWM, 8-bit , TOP=0xFF
	// CS = 1 = no prescaling
	// FOC1A = FOC1B = Force output compare
	TCCR1A = 0xF1; TCCR1B = 0x09; TCCR1C = 0xC0;

	// COM2A = 3 = set on compare match
	// COM2B = 0 = no pwm output
	// WGM = 3 = Fast PWM, TOP=0xFF
	// CS = 1 = no prescaling
	// FOC1A = FOC1B = Force output compare
	TCCR2A = 0xC3; TCCR2B = 0xC1;
#elif defined(ARDUINO_attiny)
	// COM0A = 3 = set on compare match
	// COM0B = 0 = no pwm output
	// WGM = 3 = Fast PWM, TOP=0xFF
	// CS = 1 = no prescaling
	// FOC1A = FOC1B = Force output compare
	TCCR0A = 0xF1; TCCR0B = 0x09;

	// CTC = 0 = no counter reset on compare match
	// COM1A = COM1B = 3 = set on compare match
	// WGM = 3 = Fast PWM, TOP=0xFF
	// CS = 1 = no prescaling
	// FOC1A = FOC1B = Force output compare
	TCCR1 = 0xC3; GTCCR = 0x3C;
	OCR1C = 0xFF;
#else
#error device unsupported
#endif
}

#ifdef BY_6
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

		displaySubStep = 1;
	break;
	case 1:
		// A = '1';
		port |= OUT_A; OCR_OUT_A = 0; ddr |= OUT_A;

		// C = leds[1] ? '0' : 'z';
		if (leds[1] > 0) {
			ddr |= OUT_C;
			OCR_OUT_C = leds[1];
		}
		displaySubStep = 2;
	break;
	case 2:
		// B = '1';
		port |= OUT_B; OCR_OUT_B = 0; ddr |= OUT_B;

		// A = leds[3] ? '0' : 'z';
		if (leds[3] > 0) {
			ddr |= OUT_A;
			OCR_OUT_A = leds[3];
		}

		displaySubStep = 3;
	break;
	case 3:
		// B = '1';
		port |= OUT_B; OCR_OUT_B = 0; ddr |= OUT_B;

		// C = leds[4] ? '0' : 'z';
		if (leds[4] > 0) {
			ddr |= OUT_C;
			OCR_OUT_C = leds[4];
		}
		displaySubStep = 4;
	break;
	case 4:
		// C = '1';
		port |= OUT_C; OCR_OUT_C = 0; ddr |= OUT_C;

		// A = leds[0] ? '0' : 'z';
		if (leds[0] > 0) {
			ddr |= OUT_A;
			OCR_OUT_A = leds[0];
		}

		displaySubStep = 5;
	break;
	case 5:
		// C = '1';
		port |= OUT_C; OCR_OUT_C = 0; ddr |= OUT_C;

		// B = leds[5] ? '0' : 'z';
		if (leds[5] > 0) {
			ddr |= OUT_B;
			OCR_OUT_B = leds[5];
		}

		displaySubStep = 0;
	break;
	}
	PORT = port;
	DDR  = ddr;
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

