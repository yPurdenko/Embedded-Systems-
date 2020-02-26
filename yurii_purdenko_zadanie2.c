#include <avr/io.h>
#include <stdio.h>
#include <inttypes.h>
#include <util/delay.h>
#include <string.h>

#define BAUD 9600                            // baudrate
#define UBRR_VALUE ((F_CPU) / 16 / (BAUD)-1) // zgodnie ze wzorem
#define LED_DDR DDRD
#define LED_PORT PORTD

#define DOT_TIME (1)
#define COM_TIME (2)
#define DASH_TIME (3)
#define DELAY_UNIT (100)
#define INTER_WORD_TIME (7)
#define INTER_CHAR_TIME (3)

// inicjalizacja UART
void uart_init()
{
    // ustaw baudrate
    UBRR0 = UBRR_VALUE;
    // wyczyść rejestr UCSR0A
    UCSR0A = 0;
    // włącz odbiornik i nadajnik
    UCSR0B = _BV(RXEN0) | _BV(TXEN0);
    // ustaw format 8n1
    UCSR0C = _BV(UCSZ00) | _BV(UCSZ01);
}

// transmisja jednego znaku
int uart_transmit(char data, FILE *stream)
{
    // czekaj aż transmiter gotowy
    while (!(UCSR0A & _BV(UDRE0)))
        ;
    UDR0 = data;
    return 0;
}

// odczyt jednego znaku
int uart_receive(FILE *stream)
{
    // czekaj aż znak dostępny
    while (!(UCSR0A & _BV(RXC0)))
        ;
    return UDR0;
}

FILE uart_file;

int main()
{
    // zainicjalizuj UART
    uart_init();
    // skonfiguruj strumienie wejścia/wyjścia
    fdev_setup_stream(&uart_file, uart_transmit, uart_receive, _FDEV_SETUP_RW);
    stdin = stdout = stderr = &uart_file;
    LED_DDR = 0xff;
    char texto[37] = {
        'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l',
        'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x',
        'y', 'z', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', ' '};
    char m[37][10] = {
        {".-,"}, {"-...,"}, {"-.-.,"}, {"-..,"}, {".,"}, {"..-.,"}, {"--.,"}, {"....,"}, {"..,"}, {".---,"}, {"-.-,"}, {".-..,"}, {"--,"}, {"-.,"}, {"---,"}, {".--.,"}, {"--.-,"}, {".-.,"}, {"...,"}, {"-,"}, {"..-,"}, {"...-,"}, {".--,"}, {"-..-,"}, {"-.--,"}, {"--..,"}, {".----,"}, {"..---,"}, {"...--,"}, {"....-,"}, {".....,"}, {"-....,"}, {"--...,"}, {"---..,"}, {"---.,"}, {"-----,"}, {"/"}};
    while (1)
    {
        char str[500];
        scanf("%s", str);
        for (int i = 0; i < strlen(str); i++)
        {
            for (int j = 0; j < 37; j++)
            {
                if (texto[j] == str[i])
                {
                    for (int k = 0; k < strlen(m[j]); k++)
                    {
                        if (m[j][k] == '.')
                        {
                            LED_PORT = 0b00001000;
                            _delay_ms(DOT_TIME * DELAY_UNIT);
                            LED_PORT = 0b00000000;
                        }
                        else if (m[j][k] == ',')
                        {
                            LED_PORT = 0b00001000;
                            _delay_ms(COM_TIME * DELAY_UNIT);
                            LED_PORT = 0b00000000;
                        }
                        else if (m[j][k] == '-')
                        {
                            LED_PORT = 0b00001000;
                            _delay_ms(DASH_TIME * DELAY_UNIT);
                            LED_PORT = 0b00000000;
                        }
                        else if (m[j][k] == '/')
                        {
                            LED_PORT = 0b00001000;
                            _delay_ms(INTER_WORD_TIME * DELAY_UNIT);
                            LED_PORT = 0b0000000;
                        }
                        _delay_ms(INTER_CHAR_TIME * DELAY_UNIT);
                    }
                }
            }
        }
    }
}