#include <Arduino.h>

#include "Animation.h"
#include "myStrip.h"

#ifndef DEFAULT_BAUDRATE
	#define DEFAULT_BAUDRATE 115200
#endif

typedef long (*effectFunction)(long);

extern long EffectDrops(long);
extern long EffectRomain(long);
extern long EffectAll(long);
extern long EffectStack(long);
extern long EffectPeaks(long);
extern long EffectShade(long);
extern long EffectScroll(long);
extern long EffectOnOff(long);
