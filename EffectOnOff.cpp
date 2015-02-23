#include "Guirlande.h"
#include "Animation.h"

//package piif.guirlande;
//
//import static piif.guirlande.Guirlande.*;
//import static piif.guirlande.Color.*;

class EffectOnOff implements Animation {

	static boolean flag;
	static int h;
	static byte position;

	long doStep(long step) {
		if (step == 0) {
			flag = HIGH;
			h = 1;
			position = 0;
		}
		if (flag) {
			stripSetH(position, h);
			h = (h + 3) % 256;
			if (h == 0) {
				h++;
			}
		} else {
			stripSetH(position, 0);
		}
	    stripUpdate();
	    position++;
		if (position == STRIP_LEN) {
			position = 0;
			flag = flag ? LOW : HIGH;
		}
		if (step == STRIP_LEN *2 * 5) {
			return -1;
		}
		return 5;
	}
};
