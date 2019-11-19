#include <Arduino.h>

#define MAX_INTENSITY 255

extern byte leds[6];

// following constants are used for WITH_NUMS and WITH_ANIM modes
#undef BY_3
// define constant above to test display in 3 steps
// should be more efficient, but intensity may vary depending on which LEDs are on or off
#define BY_6
// define constant above to test display in 6 steps
// user inputs up to 6 values between 1 and 6
// listed LED should be set "on"

// undef to use on/off mode , def to use variable intensity thru PWM
#define WITH_PWM

#define PIN_A 9
#define PIN_B 10
#define PIN_C 11

// masks for DDRB and PORTB
// DDRB = 0 for output , 1 for INPUT
// PORTB = output
// thus :
//  DDRB=1 , PORTB=1 => +5
//  DDRB=1 , PORTB=0 => GND
//  DDRB=0 , PORTB=? => Z

#define OUT_A 0x02
#define OUT_B 0x04
#define OUT_C 0x08

#ifdef WITH_PWM
#define OCR_OUT_A OCR1A
#define OCR_OUT_B OCR1B
#define OCR_OUT_C OCR2A
#endif

extern void lineInit();
extern void lineSet(byte value);
extern void displayStep();

// version to use from lineEffect functions
extern void lineSet(byte leds[], byte value);
