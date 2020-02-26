#include <avr/io.h>
#include <util/delay.h>

#define LED_DDR DDRD
#define LED_PORT PORTD

int main()
{
  UCSR0B &= ~_BV(RXEN0) & ~_BV(TXEN0);
  LED_DDR = 0xff;
  uint8_t i = 0;
  while (1)
  {
    i = 0;
    while (i < 8)
    {
      LED_PORT = (1 << i);
      _delay_ms(50);
      i += 1;
    }
    while (i > 0)
    {
      LED_PORT = (1 << i);
      _delay_ms(50);
      i -= 1;
    }
      LED_PORT = 0b00011000;
      _delay_ms(50);
      LED_PORT = 0b00100100;
      _delay_ms(50);
      LED_PORT = 0b01000010;
      _delay_ms(50);
      LED_PORT = 0b10000001;
      _delay_ms(50);
  }
  return 0;
}
