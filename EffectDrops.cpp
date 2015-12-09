#include "Guirlande.h"

static Color colors[] = {
	{ g:255, r:  0 ,b:  0 },
	{ g:255, r:255 ,b:  0 },
	{ g:  0, r:255 ,b:  0 },
	{ g:  0, r:255 ,b:255 },
	{ g:  0, r:  0 ,b:255 },
	{ g:255, r:  0 ,b:255 },
	{ g:255, r:255 ,b:255 },
};
#define NB_COLORS (sizeof(colors)/sizeof(colors[0]))

#define NB_DROPS 4
static byte positions[] = { 0, 20, 40, 60 }; // loop modulo 80
static byte colorIndexes[] = { 0, 1, 2, 3 };
static byte nextColor = 4;

static void displayDrop(byte position, byte color) {
	Color c = colors[color];
	for (byte p = position, i = 10; i; i--, p--) {
		if (p >= 0 && p < 60) {
			stripSetColor(p, c);
		}
		c.r >>= 1;
		c.g >>= 1;
		c.b >>= 1;
	}
}

long EffectDrops(long step) {
	Color color;

	for(byte i = 0; i < NB_DROPS; i++) {
		displayDrop(positions[i], colorIndexes[i]);
		positions[i]++;
		if (positions[i] > 80) {
			positions[i] = 0;
			colorIndexes[i] = nextColor;
			nextColor = (nextColor + 1) % NB_COLORS;
		}
	}
	stripUpdate();
	if (step < 200) {
		return 75;
	}
	return -1;
}
