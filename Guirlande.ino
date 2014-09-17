#ifdef PIF_TOOL_CHAIN
	#include <Arduino.h>
	// other includes with full pathes
	// example : #include "led7/led7.h"
#else
	// other includes with short pathes
	// example : #include "led7.h"
#endif

#ifndef DEFAULT_BAUDRATE
	#define DEFAULT_BAUDRATE 115200
#endif

#include <Adafruit_NeoPixel.h>

#define STRIP_PIN 8
#define STRIP_LEN 60

Adafruit_NeoPixel strip = Adafruit_NeoPixel(STRIP_LEN, STRIP_PIN, NEO_GRB + NEO_KHZ800);

void setup(void) {
	Serial.begin(DEFAULT_BAUDRATE);
	pinMode(13, OUTPUT);
	strip.begin();
	Serial.println("ready"); Serial.flush();
	strip.show(); // Initialize all pixels to 'off'
	for(int i = 0; i < STRIP_LEN; i++) {
		strip.setPixelColor(i, strip.Color(255, 255, 255));
	}
	Serial.println("white"); Serial.flush();
	strip.show();
	delay(500);
	for(int i = 0; i < STRIP_LEN; i++) {
		strip.setPixelColor(i, strip.Color(0, 0, 0));
	}
	Serial.println("black"); Serial.flush();
	strip.show();
	delay(300);
	for(int i = 0; i < STRIP_LEN; i++) {
		strip.setPixelColor(i, strip.Color(255, 255, 255));
	}
	Serial.println("white"); Serial.flush();
	strip.show();
	delay(500);
	Serial.println("loop"); Serial.flush();
}

uint32_t HtoRGB(byte h) {
	byte r= 0, b= 0, g= 0;
	if(h > 0) {
		if (h <= 85) {
			r = h*3; g = 255 - h*3;
		} else if (h <= 170) {
			r = 255 - h*3; b = h*3;
		} else {
			g = h*3; b = 255 - h*3;
		}
	}
	return strip.Color(r, g, b);
}

bool flag = HIGH;
byte index = 0;
byte h = 0;

void loop() {
	if (flag) {
		strip.setPixelColor(index, HtoRGB(h));
		h = (h + 1) % 256;
	} else {
		strip.setPixelColor(index, 0);
	}
    strip.show();
	index++;
	if (index == STRIP_LEN) {
		index = 0;
		flag = flag ? LOW : HIGH;
	}
	delay(10);
}
