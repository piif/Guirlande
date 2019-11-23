typedef long (*lineEffectFunction)(long, byte leds[]);

extern long LineTwoLedTrain(long step, byte leds[]);
extern long LineOnThenOff(long step, byte leds[]);
extern long LineFadeAll(long step, byte leds[]);
extern long LineWave(long step, byte leds[]);
extern long LineShiftedFade(long step, byte leds[]) ;

long LineEffectAllMax(long step, byte leds[]) {
	lineSet(leds, 255);
	return 1000;
}

// first anim is "all LEDs steady on", for testing power consumption and LEDs
// excepted in "test mode", this effect is skipped
lineEffectFunction lineEffects[] = {
	LineEffectAllMax,
	LineTwoLedTrain,
	LineOnThenOff,
	LineFadeAll,
	LineWave,
	LineShiftedFade
};
