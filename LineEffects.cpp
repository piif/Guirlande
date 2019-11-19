#include "myLine.h"

void rotateRight(byte leds[]) {
	byte tmp = leds[5];
	leds[5] = leds[4];
	leds[4] = leds[3];
	leds[3] = leds[2];
	leds[2] = leds[1];
	leds[1] = leds[0];
	leds[0] = tmp;
}

// loops on 6 steps , make it 5 times => stop at step = 6x5-1 = 29
long LineTwoLedTrain(long step, byte leds[]) {
	static byte led = 0;

	if (step == 30) { return -1; }

	if (step == 0) {
		leds[0] = MAX_INTENSITY;
		leds[1] = MAX_INTENSITY >> 3;
		leds[2] = 0;
		leds[3] = 0;
		leds[4] = 0;
		leds[5] = 0;
	} else {
		rotateRight(leds);
	}

	return 250;
}

// loops on 12 steps , make it 10 times => stop at step = 12x10-1 = 120
long LineOnThenOff(long step, byte leds[]) {
	static byte led = 0;
	static byte on = MAX_INTENSITY;

	if (step == 120) { return -1; }

	if (step == 0) {
		lineSet(leds, 0);
	} else {
		leds[led] = on;

		if (led == 5) {
			led = 0;
			on = (on == MAX_INTENSITY) ? 0 : MAX_INTENSITY;
		} else {
			led++;
		}
	}
	return 100;
}

// loops on 256 steps , make it 2 times => stop at step = 512
long LineFadeAll(long step, byte leds[]) {
	if (step == 512) { return -1; }

	lineSet(leds, step);

	return 5;
}

// loops on 6 steps , make it 10 times => stop at step = 60
long LineWave(long step, byte leds[]) {
	if (step == 60) { return -1; }

	if (step == 0) {
		leds[0] = 0;
		leds[1] = MAX_INTENSITY >> 4;
		leds[2] = MAX_INTENSITY >> 2;
		leds[3] = MAX_INTENSITY;
		leds[4] = MAX_INTENSITY >> 2;
		leds[5] = MAX_INTENSITY >> 4;
	} else {
		rotateRight(leds);
	}
	return 100;
}

// loops on 256 steps , make it 2 times => stop after step 512
long LineShiftedFade(long step, byte leds[]) {
	if (step == 512) { return -1; }

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

	return 5;
}

/**
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
**/
