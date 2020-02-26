#include <avr/io.h>
#include <util/delay.h>

#define LED PB3
#define LED_DDR DDRB

void init_adc() {
  ADMUX   = _BV(REFS0); // referencja AVcc, wejście ADC0
  ADCSRA  = _BV(ADPS0) | _BV(ADPS1) | _BV(ADPS2); // preskaler 128
  ADCSRA |= _BV(ADEN); // włącz ADC
}
void init_timer2() { 
	TCCR2A |= (1 << COM2A1); //non inverting
	TCCR2A |= (1 << WGM20)  | (1 << WGM21);  //fast pwm 8bit
	TCCR2B |= (1 << CS20); // CPU clock / 1 => 31250Hz 
}

int main()
{

	init_timer2();
	init_adc();
	LED_DDR = _BV(LED);
    float a;
    uint32_t val;
    uint32_t adc;

	while (1)
	{
		ADCSRA |= (1 << ADSC); 
		while (ADCSRA & (1 << ADSC));
		adc = ADC;
		a = (adc * 5.0) / 1023.0;
		val = (a * 47000.0) / (5.0 - a);
		if (val < 300000){
			OCR2A = ((val / 300000.0)*255) ;
		}else{
			OCR2A = 1;
		}
		
	}
}