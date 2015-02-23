#include <Guirlande.h>
#include <Animation.h>

//package piif.guirlande;
//
//import static piif.guirlande.Guirlande.*;
//import static piif.guirlande.Color.*;

class EffectStack implements Animation {
	int stage, position;

	long doStep(long step) {
		if (step == 0) {
			stage = STRIP_LEN - 1;
			position = -1;
			stripOff();
		}

		if (position != -1) {
			stripSetH(position, 0);
		}
		position++;
		stripSetHL(position, 255 - (stage * 4), 100 - stage);
		if (position == stage) {
			if (stage == 0) {
				return -1;
			}
			stage--;
			position = -1;
		}
		stripUpdate();

		return 2;
	}
};
