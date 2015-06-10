
/*
 NC      1   8   +5V
 KEY0    2   7   NC
 KEY1    3   6   LAMP PWM2
 GND     4   5   LAMP PWM1
 
 */

#include <avr/io.h>
#include <avr/delay.h>


//------------------------------------------------------------------------------------------------------------

void init(){
    // ADC
    ADMUX = (0<<REFS0); //VCC reference
    ADCSRA = (1<<ADEN)| (1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0); //ADC enable, prescaler 128
 
    // PWM
	DDRB |= (1<<PB0)|(1<<PB1); // PWM-outputs

	TCCR0A = (1<<COM0A1)|(1<<COM0B1)|(1<<WGM01)|(1<<WGM00); // fast PWM
	TCCR0B = (1<<CS02); // counter clock divider 256
	OCR0A = 255;
    OCR0B = 255;
}

// pre: input PB3 (ADC3) and PB4 (ADC2). PB2 (ADC1) is used as reference, PB0 and PB1 for PWM
uint16_t sensePad(uint8_t adcPin){
    int16_t measurement1, measurement2;
    uint8_t portPin;
    if (adcPin == 3) {
        portPin = PB3;
    } else {
        portPin = PB4;
    }

    // first measurement: adcPin low, S/H high
    ADMUX = (0<<REFS0) | 0x01; // set ADC sample+hold condensator to the free PB2 (ADC1) 

    PORTB |= (1<<PB2); //PB2/ADC1 ref/ S/H higt
    DDRB |= (1<<portPin) | (1<<PB2); // both output: adcPin low, S/H (ADC1) high
    _delay_us(32);
    DDRB &= ~((1<<portPin) | (1<<PB2));
    PORTB &= ~((1<<portPin) | (1<<PB2));

    ADMUX = (0<<REFS0) | (adcPin & 0x03); // read extern condensator from adcPin
    ADCSRA |= (1<<ADSC); // start conversion
    while (!(ADCSRA & (1 << ADIF))); // wait for conversion complete
    ADCSRA |= (1 << ADIF); // clear ADIF
    measurement1=ADC;

    // second measurement: adcPin high, S/H low
    ADMUX = (0<<REFS0) | 0x01; // set ADC sample+hold condensator to the free PB2 (ADC1)

    PORTB |= (1<<portPin); // sensePad/adcPin high
    DDRB |= (1<<portPin) | (1<<PB2); // both output: adcPin high, S/H (ADC1) low
    _delay_us(32);
    DDRB &= ~((1<<portPin) | (1<<PB2));
    PORTB &= ~((1<<portPin) | (1<<PB2));

    ADMUX = (0<<REFS0) | (adcPin & 0x03); // read extern condensator from adcPin
    ADCSRA |= (1<<ADSC); // start conversion
    while (!(ADCSRA & (1 << ADIF))); // wait for conversion complete
    ADCSRA |= (1 << ADIF); // clear ADCIF
    measurement2=ADC;


    return (measurement2 - measurement1)+1023;
}

uint8_t getMultiplicator(int16_t value, uint16_t maxValue){
    if (maxValue<64){
        value=value*4;
    } else if (maxValue < 86){
        value= value*3;
    } else if (maxValue < 103) {
        value = (value/2)*5;
    } else if (maxValue<128){
        value=value*2;
    } else if (maxValue < 154){
        value = (value/3)*5;
    } else if (maxValue<171){
        value = (value/2)*3;
    } else if (maxValue<205){
        value = (value/4)*5;
    } else {
        value=(value/5)*4;
    }
    return value;
}

//------------------------------------------------------------------------------------------------------------
int main(void) {
	init();
	int16_t senseValue1, senseValue2;
    int16_t refMin1, refMin2;
    uint16_t refMax1, refMax2;
    refMin1 = sensePad(2);
    refMin1 = sensePad(2);
    refMin2 = sensePad(3);
    refMin2 = sensePad(3);

    for(;;){
        PORTB &= ~(1<<PB3);

        senseValue1 = sensePad(2);
        senseValue1 = senseValue1 - refMin1;
        if (senseValue1<0){
            senseValue1=0;
        }
        if (senseValue1>refMax1) {
            refMax1=senseValue1;
        }
        senseValue1 = getMultiplicator((uint8_t) senseValue1, refMax1);

        senseValue2 = sensePad(3);
        senseValue2 = senseValue2 - refMin2;
        if (senseValue2<0){
            senseValue2=0;
        }
        if (senseValue2>refMax2) {
            refMax2=senseValue2;
        }
        senseValue2 = getMultiplicator((uint8_t) senseValue2, refMax2);

        OCR0B = senseValue1;
        OCR0A = senseValue2;
        
    }
    return 0;   /* never reached */
}


