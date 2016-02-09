#include <avr/io.h>
#include <util/delay.h>

// Hello world
// Blink led on PB4 (pin 3)
// Clock frequency???

/*
Programmer connections to attiny13

Prog	AVR t13
---------------
MISO	6
SCK	7
RESET	1
+5V	8
MOSI	5
GND	4
*/

int main(void)
{
    const int msDelay = 100;

    // PortB pin4 to output (set bit to 1 using SHL)
    DDRB = 1<<DDB4;

    // PortB to low
    PORTB = 0;

    while (1) {
        // XOR on pin 4
        PORTB ^= 1<<PB4;
        _delay_ms(msDelay);
    }

    return 0;
}
