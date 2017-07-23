/*
********************************************************************************
* File        	: EX3_1.C
* By          	: FunFun Yoo
*
* Description 	: Example for External Interrupt (Trigger : Rising Edge)
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
//#include <util/delay.h>

#define DDR_LED		DDRC
#define PORT_LED	PORTC

#define DDR_KEY		DDRB
#define PIN_KEY		PINB

#define CPU_CLOCK	16000000
#define TICKS_PER_SEC	1000


volatile unsigned int tic_time;
volatile unsigned int trigger_flag;


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

// INT0 : External Interrupt
ISR(INT0_vect)
{
    trigger_flag = 1;
}


// GPIO Init
void init_gpio()
{
    DDR_LED |= (1 << PORT7);
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

// External Interrupt Init
void init_trigger()
{
    EICRA = (1 << ISC01) | (1 << ISC00);
    EIMSK = (1 << INT0);
}


int main(void)
{
    int i;

    // --- GPIO ----
    init_gpio();

    // ---- Timer/Counter0 ----
    init_timer();

    // ---- External Interrupt ----
    init_trigger();

    // ---- Global Interrupt : Enable ----
    sei();

    for (;;)
    {
        if (trigger_flag)
        {
            trigger_flag = 0;

            for(i=0 ; i < 5 ; i++)
	    {
	        PORT_LED = ~(1 << PORT7);
	        delay_ms(100);
	        PORT_LED = (1 << PORT7);
		delay_ms(100);
	    }
        }
    }
}

