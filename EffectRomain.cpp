#include "Guirlande.h"
#include "Animation.h"

//package piif.guirlande;
//
//import static piif.guirlande.Guirlande.*;
//import static piif.guirlande.Color.*;

class EffectRomain implements Animation {
	int stage, position;
	static const int middle = STRIP_LEN / 2;

	long doStep(long step) {
//		println("doStep", step);
		if (step == 0) {
//			for(int i = 0; i < STRIP_LEN; i++) {
//				stripSetH(i, i * 4);
//			}
//			position = 0;
//			stage = 0;
			// startup : start with black strip
			stage = middle;
			position = 0;
			stripOff();
			stripSetColor(middle, COLOR_WHITE);
			stripUpdate();

			return 4;
		}

		if (stage != 0) {
			// first part : fill by center
	
			if (position != 0) {
				stripSetColor(middle + position, COLOR_BLACK);
				stripSetColor(middle - position, COLOR_BLACK);
			}
			position++;
			stripSetHL(middle + position, 255 - (stage * 8), 100 - 2 * stage);
			stripSetHL(middle - position, 255 - (stage * 8), 100 - 2 * stage);
			if (position == stage) {
				stage--;
				if (stage == 0) {
					// last stage : position == stage == 0
					return 10;
				}
				position = 0;
			}
			stripUpdate();
			return 4;
		} else {
			// second part : explosion
			// first step here : stage = 0, position = 0
			position++;
			// steps 0 thru middle
			if (position == middle) {
				return -1;
			}
			int nbTo = middle / (position + 1);
			int to = nbTo * (position + 1), from = nbTo * position;
//			println("2nd part, position = ", position, " from ", from, " to ", to);
			for (int i = middle; i >= 0; i--) {
				if (i > to) {
					stripSetH(middle + i, 0);
					stripSetH(middle - i, 0);
				} else {
					stripSetH(middle + to, stripGetH(middle + from));
					stripSetH(middle - to, stripGetH(middle - from));
					from -= position;
					to -= position + 1;
				}
			}
			stripUpdate();
			return 10;
		}

	}
};
