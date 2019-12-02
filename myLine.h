#include "Arduino.h"

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

#if defined(ARDUINO_AVR_UNO)

#define PIN_A 8
#define PIN_B 9
#define PIN_C 10

// masks for DDRx and PORTx
// DDRB = 0 for output , 1 for INPUT
// PORTB = output
// thus :
//  DDRB=1 , PORTB=1 => +5
//  DDRB=1 , PORTB=0 => GND
//  DDRB=0 , PORTB=? => Z

#define OUT_A 0x01
#define OUT_B 0x02
#define OUT_C 0x04

#define PORT PORTB
#define DDR DDRB
#define OCR_OUT_B OCR1A
#define OCR_OUT_C OCR1B

#elif defined(ARDUINO_attiny)

#define PIN_A 0 // pin 5
#define PIN_B 1 // pin 6
#define PIN_C 4 // pin 3

// masks for DDRx and PORTx
// DDRB = 0 for output , 1 for INPUT
// PORTB = output
// thus :
//  DDRB=1 , PORTB=1 => +5
//  DDRB=1 , PORTB=0 => GND
//  DDRB=0 , PORTB=? => Z

#define OUT_A 0x01
#define OUT_B 0x02
#define OUT_C 0x10

#define PORT PORTB
#define DDR DDRB
#define OCR_OUT_B OCR1A
#define OCR_OUT_C OCR1B

#else
#error device unsupported
#endif

extern void lineInit();
extern void lineOff();
extern void lineSet(byte value);
extern void displayStep();

// version to use from lineEffect functions
extern void lineSet(byte leds[], byte value);
