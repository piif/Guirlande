## TODO : integrate this command in makefiles
../../ArdDude/etc/ad.sh $1 D:/SRC/Arduino/hardware/tools/avr/bin/avrdude -pm328p -carduino -Pcom5 -Uflash:w:Uno/ArduinoExample.hex:a "-CC:/Program Files (x86)/arduino-1.0.3/hardware/tools/avr/etc/avrdude.conf"
