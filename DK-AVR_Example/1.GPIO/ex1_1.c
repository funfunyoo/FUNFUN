/*
********************************************************************************
* File        	: EX1_1.C
* By          	: FunFun Yoo
*
* Description 	: Example for GPIO (LED)
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

int main(void)
{
    int i, j;
    unsigned char led_status = (1<<PORT4);

    DDR_LED |= (1<<PORT4);
    PORT_LED = 0xFF;

    for (;;)
    {
        led_status = ~led_status;
        PORT_LED = led_status;

        // just for time delay...
        for (i=0 ; i < 5000 ; i++)
            for (j=0 ; j < 500 ; j++);
    }
}

