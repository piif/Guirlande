#include <strip.h>

#if defined(ARDUINO_AVR_UNO)
#define STRIP_PIN A0
#elif defined(ARDUINO_attiny)
#define STRIP_PIN A3 // = pin 2
#endif

#define STRIP_LEN 59

#define COLOR_BLACK ((Color){g:0,r:0,b:0})
#define COLOR_RED   ((Color){g:0,r:255,b:0})
#define COLOR_GREEN ((Color){g:255,r:0,b:0})
#define COLOR_BLUE  ((Color){g:0,r:0,b:255}})
#define COLOR_WHITE ((Color){g:255,r:255,b:255})
