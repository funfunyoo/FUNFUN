/*
********************************************************************************
* File        	: EX2_2.C
* By          	: FunFun Yoo
*
* Description 	: Example for Timer/Counter (Output Compare Match Interrupt)
*
* Copyright(c) 2017 DrimAES Inc. All rights are reserved.
*
*
* 2017.01.17	: Last modified
*
********************************************************************************
*/

#include <avr/io.h>
#include <avr/interrupt.h>

#define DDR_LED		DDRD
#define PORT_LED	PORTD

#define CPU_CLOCK	16000000
#define TICKS_PER_SEC	1000


volatile unsigned int tic_time;


// 8-bit Timer/Counter0 Output Compare Match Interrupt : OCIE0A
ISR(TIMER0_COMPA_vect)
{
    tic_time++;
}

// delay function
void delay_ms(unsigned int msec)
{
    tic_time = 0;

    while (msec > tic_time);
}

// GPIO Init
void init_gpio()
{
    DDR_LED |= (1<<PORT4);
    PORT_LED = 0xFF;
}

// Timer/Counter Init
void init_timer()
{
    // Waveform Generation Mode : CTC mode, Clock : clk/64 (prescaler)
    TCCR0B = (0 << WGM02) | (0 << CS02) | (1 << CS01) | (1 << CS00) ;
    TCCR0A =             (1 << WGM01) | (0 << WGM00);
    TCNT0 = 0; 
    // Output Compare Match Register
    OCR0A = (CPU_CLOCK / TICKS_PER_SEC / 64) - 1;   // (n - 1)
    // Timer/Counter0 Overflow Interrupt : Enable
    TIMSK0 = (0 << OCIE0B) | (1 << OCIE0A) | (1 << TOIE0);
}


int main(void)
{
    unsigned char led_status = (1<<PORT4);

    // --- GPIO ----
    init_gpio();

    // ---- Timer/Counter0 ----
    init_timer();

    // ---- Global Interrupt : Enable ----
    sei();

    for (;;)
    {
        led_status = ~led_status;
        PORT_LED = led_status;

        // Timer/Counter0 : delay 
        delay_ms(1000);
    }
}

