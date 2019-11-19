typedef long (*lineEffectFunction)(long, byte leds[]);

extern long LineTwoLedTrain(long step, byte leds[]);
extern long LineOnThenOff(long step, byte leds[]);
extern long LineFadeAll(long step, byte leds[]);
extern long LineWave(long step, byte leds[]);
extern long LineShiftedFade(long step, byte leds[]) ;

#ifdef POWER_TEST
long LineEffectAllMax(long step, byte leds[]) {
	lineSet(leds, 255);
	return 1000;
}
#endif

lineEffectFunction lineEffects[] = {
#ifdef POWER_TEST
	LineEffectAllMax
#else
	LineTwoLedTrain,
	LineOnThenOff,
	LineFadeAll,
	LineWave,
	LineShiftedFade
#endif
};
