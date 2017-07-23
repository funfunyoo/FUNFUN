/*
********************************************************************************
* File        	: EX1_2.C
* By          	: FunFun Yoo
*
* Description 	: Example for GPIO (LED, Tact Switch)
*
* Copyright(c) 2017 DrimAES Inc. All rights are reserved.
*
*
* 2017.01.17	: Last modified
*
********************************************************************************
*/

#include <avr/io.h>

#define DDR_LED		DDRD
#define PORT_LED	PORTD

#define DDR_KEY		DDRB
#define PIN_KEY		PINB

int main(void)
{
    DDR_LED |= (1<<PORT4);   // ** /usr/lib/avr/include/avr/portpins.h : #define PORT4 4
    PORTD = 0xFF;

    DDRB |= (0<<PIN4) ;    // ** /usr/lib/avr/include/avr/portpins.h : #define PIN4 4

    for (;;)
    {
/*
        // Tact Switch (L) -> LED On : (L)
        if (!(PIN_KEY & 0x10))  // Key pushed : 0x10 -> 0b 0001 0000
            PORT_LED = ~(1<<PORT4);
        else
            PORT_LED = (1<<PORT4);
*/
        PORT_LED = PIN_KEY;
    }
}

