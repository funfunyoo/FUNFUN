/*
********************************************************************************
* File        	: EX5_1.C
* By          	: FunFun Yoo
*
* Description 	: Example for ADC (Potentiometer : Variable Ohm)
*
* Copyright(c) 2017 DrimAES Inc. All rights are reserved.
*
*
* 2017.01.17	: Last modified
*
********************************************************************************
*/

#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>	// itoa()

#define CPU_CLOCK       16000000

#define BAUD_RATE       19200
#define BAUD_RATE_L     (CPU_CLOCK / (16l * BAUD_RATE)) - 1
#define BAUD_RATE_H     ((CPU_CLOCK / (16l * BAUD_RATE)) - 1) >> 8

// 1 byte transmission function
void uart_send_byte(unsigned char byte)
{
    // Wait for empty transmit buffer
    while (!(UCSR1A & (1 << UDRE1)));
     
    UDR1 = byte;
}

// String transmission function
void uart_send_string(unsigned char *str, unsigned char len)
{
    int i;

    for (i = 0; i < len; i++)
    {
        if (!(*(str + i)))
	    break;
	
	uart_send_byte(*(str + i));
    }
}

// UART Init
void init_uart()
{
    // Baud rate : 19200
    UBRR1L = (unsigned char)BAUD_RATE_L;
    UBRR1H = (unsigned char)BAUD_RATE_H;
    // 8-N-1 : Data 8-bit, No parity, 1 Stop bit
    UCSR1C =                 (1 << UCSZ11) | (1 << UCSZ10) | (0 << UPM11) | (0 << UPM10) | (0 << USBS1);
    UCSR1B = (0 << UCSZ12) | (1 << TXEN1) | (1 << RXEN1);
}

// Read ADC data
unsigned int get_adc_data(void)
{
    // Start ADC Conversion
    ADCSRA |= (1 << ADSC);

    // Wait until complete adc conversion
    while (ADCSRA & (1 << ADSC));

    return ADC;
}

// ADC Init
void init_adc()
{
    // AVCC with external capacitor on AREF pin
    ADMUX |= (0 << REFS1) | (1 << REFS0);
    // DK-AVR Schematic : PF0 (ADC0) -- PF4_VR -- VR_ADC, Single Ended Input
    ADMUX |= (0 << MUX4) | (0 << MUX3) | (0 << MUX2) | (0 << MUX1) | (0 << MUX0);
    // Enable ADC, Division factor = 128 (= 125kHz) : 50kHz < 125kHz < 200kHz
    ADCSRA |= (1 << ADEN) | (1 << ADPS2) | (1 <<ADPS1 ) | (1 << ADPS0);
}


int main(void)
{
    unsigned int adc_data;
    unsigned char ADC_RESULT[10];

    // ---- UART ----
    init_uart();

    // ---- ADC ----
    init_adc();


    for (;;)
    {
        adc_data = get_adc_data();
	
	itoa(adc_data, ADC_RESULT, 10);
        
	uart_send_string(ADC_RESULT, 10);
	uart_send_string("\r\n", 2);

	_delay_ms(500);
    }
}

