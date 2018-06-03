CC=avr-gcc
CFLAGS=-Wall -O2 -mmcu=atmega328p

all:	helloworld.hex

%.hex: %.out
	avr-objcopy -O ihex -R .eeprom $< $@

%.out: %.c
	$(CC) $(CFLAGS)  $< -o $@

clean:
	rm -rf *.o *.hex *.out

upload:	helloworld.hex
	avrdude -c usbasp -p m328p -U flash:w:$<

.PHONY:	clean

#fuse:
#	sudo avrdude -c avrisp -P /dev/cu.usbmodem1441 -p m328p -U lfuse:w:0x7a:m -U hfuse:w:0xff:m
