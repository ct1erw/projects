CC=gcc
CFLAGS=-Wall -g
DEPS=

%.o:	%.c $(DEPS)
	$(CC) $(CFLAGS) -c -o $@ $<

all:	gpib

gpib:	gpib.o
	$(CC) -o $@ $<

clean:
	rm -f gpib.o
