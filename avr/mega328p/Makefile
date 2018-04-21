all: a.hex

a.hex: a.out
	avr-objcopy -O ihex -R .eeprom a.out a.hex

a.out: helloworld.c
	avr-gcc -Wall -O2 -mmcu=atmega328p helloworld.c -o a.out

clean:
	rm -rf *.o *.hex a.out

upload:
	avrdude -c usbasp -p m328p -U flash:w:a.hex

#fuse:
#	sudo avrdude -c avrisp -P /dev/cu.usbmodem1441 -p m328p -U lfuse:w:0x7a:m -U hfuse:w:0xff:m

