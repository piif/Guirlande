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

#ifdef WITH_PWM
	TCCR1A = 0xF1; TCCR1B = 0x09; TCCR1C = 0xC0;
	TCCR2A = 0xC3; TCCR2B = 0xC1;
	//	OCR_OUT_A = 50;
	//	OCR_OUT_B = 128;
	//	OCR_OUT_C = 200;
	//	DDRB |= OUT_A | OUT_B | OUT_C;
#endif
}

#ifdef BY_6
void displayStep() {
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

#else // BY_6

void displayStep() {
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

#endif // BY_6

