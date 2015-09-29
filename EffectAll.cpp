#include <Guirlande.h>
#include <Animation.h>

class EffectAll : public Animation {
	byte stage;
	byte r, g, b;

	virtual long doStep(long step) {
		if (step == 0) {
			stage = 0;
			r = 0; g = 0; b = 0;
		} else if (step % 255 == 0) {
			stage++;
			if (stage == 8) {
				return -1;
			}
		}
		stripAll(r, g, b);
		switch(stage) {
			case 0: r++; break;
			case 1: g++; break;
			case 2: r--; break;
			case 3: b++; break;
			case 4: g--; break;
			case 5: r++; break;
			case 6: g++; break;
			case 7: r--; g--; b--; break;
		}
		return 5;
	}
	void status() {
		Serial.print("EffectAll stage "); Serial.println(stage);
		Serial.print(r); Serial.print(", ");
		Serial.print(g); Serial.print(", ");
		Serial.println(b);
	};
	virtual ~EffectAll() {};
};
