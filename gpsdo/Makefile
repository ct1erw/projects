CC=gcc
CFLAGS=-Wall -g
DEPS=

%.o:	%.c $(DEPS)
	$(CC) $(CFLAGS) -c -o $@ $<

all:	gpsdo

gpsdo:	gpsdo.o
	$(CC) -o $@ $<

clean:
	rm -f gpsdo.o gpsdo
