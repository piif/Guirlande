/**
 * Simplified version of Adafruit_NeoPixel lib
 * constraints :
 * - only for WS2812 @ 800KHz with GRB LEDs
 * - caller must specify some defines
 * - only one strip is handled
 */

#include <Arduino.h>

#if !defined(FROM_STRIP_C) && !defined(STRIP_LEN)
#error STRIP_LEN must be defined
#endif

typedef struct _color {
	// fields in the order needed to send raw data
	byte g;
	byte r;
	byte b;
} Color;

void stripInit(int len, byte pin);
void stripSetRGB(int position, byte r, byte g, byte b);
void stripSetColor(int position, Color c);
void stripSetH(int position, byte h);
void stripSetHL(int position, byte h, byte l);
Color stripGetColor(int position);
byte stripGetH(int position);

void stripUpdate();

void HtoRGB(byte h, Color *c);
void HLtoRGB(byte h, byte l, Color *c);
byte RGBtoH(Color c);

#ifdef FROM_STRIP_C
extern Color strip[];
#else
Color strip[STRIP_LEN];
#endif
