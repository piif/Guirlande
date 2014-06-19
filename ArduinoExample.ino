#include "led7/led7.h"

void setup(void) {
	pinMode(13, OUTPUT);
}

bool flag = LOW;
void loop() {
	delay(500);
	flag = flag ? LOW : HIGH;
	digitalWrite(13, flag);
}
