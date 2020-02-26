#include <avr/io.h>
#include <stdio.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#define BAUD 9600
#define UBRR_VALUE ((F_CPU) / 16 / (BAUD)-1)
void uart_init()
{
    UBRR0 = UBRR_VALUE;
    UCSR0A = 0;
    UCSR0B = _BV(RXEN0) | _BV(TXEN0);
    UCSR0C = _BV(UCSZ00) | _BV(UCSZ01);
}

int uart_transmit(char data, FILE *stream)
{
    while (!(UCSR0A & _BV(UDRE0)))
        ;
    UDR0 = data;
    return 0;
}

int uart_receive(FILE *stream)
{
    while (!(UCSR0A & _BV(RXC0)))
        ;
    return UDR0;
}

FILE uart_file;

void adc_init()
{
    ADMUX |= _BV(REFS0);
    DIDR0 = _BV(ADC0D) | _BV(ADC1D);
    ADCSRA |= _BV(ADPS0) | _BV(ADPS1) | _BV(ADPS2);
    ADCSRA |= _BV(ADEN);
}

void timer1_init()
{
    TCCR1A |= _BV(COM1A1) | _BV(COM1A0);
    TCCR1B |= _BV(WGM13) | _BV(CS11);
    ICR1 = 1024;
    DDRB |= _BV(PB1);
}

ISR(TIMER1_CAPT_vect)
{
    TIMSK1 &= ~_BV(ICIE1);
    ADCSRA |= _BV(ADSC);
}

ISR(TIMER1_OVF_vect)
{
    ADCSRA |= _BV(ADSC);
    TIMSK1 &= ~_BV(TOIE1);
}

int main()
{
    uart_init();
    fdev_setup_stream(&uart_file, uart_transmit, uart_receive, _FDEV_SETUP_RW);
    stdin = stdout = stderr = &uart_file;
    sei();
    adc_init();
    timer1_init();
    uint16_t ADC_result = 0;
    uint8_t start_up = 1, counter = 0;
    uint16_t ADC_lower, ADC_upper;
    while (1)
    {
        ADMUX &= ~_BV(MUX0);
        ADCSRA |= _BV(ADSC);
        while (!(ADCSRA & _BV(ADIF)))
            ;
        ADCSRA |= _BV(ADIF);
        ADC_result = 1023 - ADC;
        if (ADC_result >= 1010)
            start_up = 1;
        if (start_up && ADC_result < 975)
        {
            OCR1A = 768;
            start_up--;
        }
        else if (ADC_result < 975)
            OCR1A = ADC_result;
        else
            OCR1A = 1024;
        if (counter == 100)
        {
            ADMUX |= _BV(MUX0);
            counter = 0;
            TIMSK1 |= _BV(ICIE1);
            while (!(ADCSRA & _BV(ADIF)))
                ;
            ADCSRA |= _BV(ADIF);
            ADC_upper = ADC;
            TIMSK1 |= _BV(TOIE1);
            while (!(ADCSRA & _BV(ADIF)))
                ;
            ADCSRA |= _BV(ADIF);
            ADC_lower = ADC;

            float f1 = (float)((float)ADC_upper * 5.0)/1024.0;
            float f2 = (float)((float)ADC_lower * 5.0)/1024.0;
            printf("Napięcia H = %2.2f,L=%2.2f\r", f1, f2);
        }
        counter++;
        _delay_ms(10);
    }
}
