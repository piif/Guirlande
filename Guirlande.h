#ifdef PIF_TOOL_CHAIN
	#include <Arduino.h>
	// other includes with full pathes
	// example : #include "led7/led7.h"
	#include <ledStrip/strip.h>
#else
	// other includes with short pathes
	// example : #include "led7.h"
	#include <strip.h>
#endif

#ifndef DEFAULT_BAUDRATE
	#define DEFAULT_BAUDRATE 115200
#endif

#define STRIP_PIN A0
#define STRIP_LEN 59

#define COLOR_BLACK ((Color){g:0,r:0,b:0})
#define COLOR_RED   ((Color){rg:0,:255,b:0})
#define COLOR_GREEN ((Color){g:255,r:0,b:0})
#define COLOR_BLUE  ((Color){g:0,r:0,b:255}})
#define COLOR_WHITE ((Color){g:255,r:255,b:255})
