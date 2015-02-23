#include <Guirlande.h>
#include <Animation.h>

//package piif.guirlande;
//
//import static piif.guirlande.Guirlande.*;
//import static piif.guirlande.Color.*;

class EffectShade implements Animation {
	int h;

	long doStep(long step) {
		if (step == 0) {
			h = 17;
		}
		for(byte i = 0; i < STRIP_LEN; i++) {
			stripSetHL(i, h, (i+1) * 3);
		}

		stripUpdate();
		h = h * 4;
		return -1;
	}
};
