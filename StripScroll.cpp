#include "myStrip.h"

long EffectScroll(long step) {
	if (step == 0) {
		for(byte i = 0; i < STRIP_LEN; i++) {
			stripSetH(i, (i+1) * 4);
		}
	} else if (step == 5 * 60) {
		return -1;
	} else {
		int first = stripGetH(0);
		for (int i = 1; i < STRIP_LEN; i++) {
			stripSetH(i - 1, stripGetH(i));
		}
		stripSetH(STRIP_LEN - 1, first);
		stripUpdate();
	}

	return 25;
}
