typedef long (*stripEffectFunction)(long);

extern long EffectDrops(long);
extern long EffectRomain(long);
extern long EffectAll(long);
extern long EffectStack(long);
extern long EffectPeaks(long);
extern long EffectShade(long);
extern long EffectScroll(long);
extern long EffectOnOff(long);

#ifdef POWER_TEST
long StripEffectAllMax(long step) {
	stripAll(255, 255, 255);
	return 1000;
}
#endif

stripEffectFunction effectsStrip[] = {
#ifdef POWER_TEST
	StripEffectAllMax
#else
	EffectDrops,
	EffectAll,
	EffectStack,
	EffectRomain,
	EffectPeaks,
	EffectShade,
	EffectScroll,
	EffectOnOff
#endif
};
