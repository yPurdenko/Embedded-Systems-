#include <stdio.h>
#include <avr/io.h>
#include <util/delay.h>
#include <inttypes.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>

#define SCK_PORT PORTD4
#define MISO_PORT PORTD5
#define MOSI_PORT PORTD6

#define BAUD 9600                          // baudrate
#define UBRR_VALUE ((F_CPU)/16/(BAUD)-1)   // zgodnie ze wzorem



// inicjalizacja UART
void uart_init()
{
	// ustaw baudrate
	UBRR0 = UBRR_VALUE;
	// wyczyść rejestr UCSR0A
	UCSR0A = 0;
	// wlącz odbiornik i nadajnik
	UCSR0B = _BV(RXEN0) | _BV(TXEN0);
	// ustaw format 8n1
	UCSR0C = _BV(UCSZ00) | _BV(UCSZ01);
}

// transmisja jednego znaku
int uart_transmit(char data, FILE *stream)
{
	// czekaj az transmiter gotowy
	while(!(UCSR0A & _BV(UDRE0)));
	UDR0 = data;
	return 0;
}

// odczyt jednego znaku
int uart_receive(FILE *stream)
{
	// czekaj aż znak dostępny
	while (!(UCSR0A & _BV(RXC0)));
	return UDR0;
}

FILE uart_file;

ISR(SPI_STC_vect) 
{
	uint8_t val = SPDR;  //odbieramy wiadomość
	printf("received: %d" PRIu8 "\n", val);
}

void send_data(uint8_t data)
{
	// send bits 7..0
	for (int i = 0; i < 8; i++)
	{
		// set MOSI high if bit is 1, low if bit is 0
		if (data & 0x80)
			PORTC |= (1 << MOSI_PORT);
		else
			PORTC &= ~(1 << MOSI_PORT);

		// pulse clock to indicate that bit value should be read
		
		PORTC |= (1 << SCK_PORT);
		PORTC &= ~(1 << SCK_PORT);

		// shift byte left so next bit will be leftmost
		data <<= 1;
	}
}

void init_spi()
{
	DDRB = ( 1 << PORTB4 ); //Miso - wyjście
	SPCR = ( 1 << SPE )|( 1 << SPIE ) ;    //Włączamy przerwania i SPI
	SPDR = 0;
}

int main(void)
{ //PORTC0 - SCK PORTC1 - MISO PORTC2 - MOSI
	DDRC |= (1 << SCK_PORT) | (1 << MOSI_PORT); // Miso stays on input
	DDRB = 0;
	
	// zainicjalizuj UART
	uart_init();
	// skonfiguruj strumienie wejscia/wyjscia
	fdev_setup_stream(&uart_file, uart_transmit, uart_receive, _FDEV_SETUP_RW);
	stdin = stdout = stderr = &uart_file;
	
	init_spi();
	sei();
	for (uint8_t i = 0; i < 6; i++){
		send_data(i);		
  }
	while(1)
	{
	}
}
