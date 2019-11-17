#include "Animation.h"

static int peak;
static byte h;
static int nbPeaks;

long EffectPeaks(long step) {
	if (step == 0) {
		nbPeaks = 10;
		peak = STRIP_LEN / 7;
		h = 3;
	}
	for (int i = peak - 9; i < peak + 9; i++) {
		if (i < 0 || i > STRIP_LEN) {
			continue;
		}
		byte level = 9 - abs(9 - (step % 19));
		byte dist = abs(peak - i);
		if (level == 0) {
			stripSetH(i, 0);
		} else {
			stripSetHL(i, h, (10 - dist) * level * 2);
		}
	}
	stripUpdate();
	if (step % 19 == 18) {
		nbPeaks--;
		if (nbPeaks == 0) {
			return -1;
		}
		peak = (peak + 17) % STRIP_LEN;
		h = (h + 13) % 255 + 1;
	}
	return 25;
}
