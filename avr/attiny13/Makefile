#
#PROG_DEV=/dev/cu.usbmodem1441
#PROG_TYPE=avrisp
PROG_TYPE=usbasp
PROG_DEV=usb
#
#
TARGET=helloworld.out
OBJECTS=helloworld.c

all: helloworld.hex

%.hex: %.out
	avr-objcopy -O ihex -R .eeprom $< $@

$(TARGET): $(OBJECTS)
	avr-gcc -Wall -O2 -mmcu=attiny13 -DF_CPU=9600000 $< -o $(TARGET)

clean:
	rm -rf *.o *.hex *.out

upload: helloworld.hex
	avrdude -c $(PROG_TYPE) -P $(PROG_DEV) -p t13 -U flash:w:helloworld.hex

fuse:
	avrdude -c $(PROG_TYPE) -P $(PROG_DEV) -p t13 -U lfuse:w:0x7a:m -U hfuse:w:0xff:m

