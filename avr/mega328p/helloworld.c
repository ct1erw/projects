#define F_CPU 1600000UL

#include <avr/io.h>
#include <util/delay.h>

int main (void)
{
    DDRB |= _BV(DDB5);

    while(1)
    {
        PORTB ^= _BV(PB5);
        _delay_ms(100);
    }
}
