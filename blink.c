#include <avr/io.h>
#include <util/delay.h>

#define LED PD7
#define LED_DDR DDRD
#define LED_PORT PORTD

int main() {
  LED_DDR = _BV(LED);
  while (1) {
    LED_PORT = 0b01000000;
    _delay_ms(500);
    LED_PORT = 0b00000000;
    _delay_ms(1000);
  }
}