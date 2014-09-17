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

#define STRIP_PIN 8
#define STRIP_LEN 60
#include <strip.h>



void setup(void) {
	Serial.begin(DEFAULT_BAUDRATE);

	pinMode(13, OUTPUT);
	Serial.println("ready");
	stripInit(STRIP_LEN, STRIP_PIN);
	stripUpdate(); // Initialize all pixels to 'off'
	for(int i = 0; i < STRIP_LEN; i++) {
		stripSetRGB(i, 255, 255, 255);
	}
	Serial.println("white");Serial.flush();
	stripUpdate();
	delay(5000);
	for(int i = 0; i < STRIP_LEN; i++) {
		stripSetRGB(i, 0, 0, 0);
	}
	Serial.println("black");Serial.flush();
	stripUpdate();
	delay(3000);
	for(int i = 0; i < STRIP_LEN; i++) {
		stripSetRGB(i, 255, 255, 255);
	}
	Serial.println("white");Serial.flush();
	stripUpdate();
	delay(5000);
	Serial.println("loop");Serial.flush();
}

bool flag = HIGH;
Color color = { 0, };
byte position = 0;

void loop() {
	stripSetColor(position, color);
	stripUpdate();
	position++;
	if (position == STRIP_LEN) {
		position = 0;
		flag = flag ? LOW : HIGH;
		if (flag) {
			color.r = 255;
		} else {
			color.r = 0;
		}
	}
//	delay(5);
}
