typedef long (*stripEffectFunction)(long);

extern long EffectDrops(long);
extern long EffectRomain(long);
extern long EffectAll(long);
extern long EffectStack(long);
extern long EffectPeaks(long);
extern long EffectShade(long);
extern long EffectScroll(long);
extern long EffectOnOff(long);

long StripEffectAllMax(long step) {
	stripAll(255, 255, 255);
	return 1000;
}

// first anim is "all LEDs steady white", for testing power consumption and LEDs
// excepted in "test mode", this effect is skipped
stripEffectFunction effectsStrip[] = {
	StripEffectAllMax,
	EffectDrops,
	EffectAll,
	EffectStack,
	EffectRomain,
	EffectPeaks,
	EffectShade,
	EffectScroll,
	EffectOnOff
};
