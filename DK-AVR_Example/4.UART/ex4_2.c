/*
********************************************************************************
* File        	: EX4_2.C
* By          	: FunFun Yoo
*
* Description 	: Example for UART (MMI : Man-Machine Interface)
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
#include <stdlib.h>	// atoi() 

#define DDR_LED		DDRC
#define PORT_LED	PORTC

#define DDR_KEY		DDRB
#define PIN_KEY		PINB

#define CPU_CLOCK	16000000
#define TICKS_PER_SEC	1000

#define BAUD_RATE	19200
#define BAUD_RATE_L	(CPU_CLOCK / (16l * BAUD_RATE)) - 1
#define BAUD_RATE_H	((CPU_CLOCK / (16l * BAUD_RATE)) - 1) >> 8

#define BS		0x08	// ASCII : Back Space
#define CR		0x0D	// ASCII : Carriage Return (Enter key)

#define UART_RX_BUFLEN	128


volatile unsigned int tic_time;

// 8-bit Timer/Counter0 Overflow Interrupt
ISR(TIMER0_OVF_vect)
{
    tic_time++;

    TCNT0 = 256 - (CPU_CLOCK / TICKS_PER_SEC / 64);
}

// delay function
void delay_ms(unsigned int msec)
{
    tic_time = 0;

    while (msec > tic_time);
}


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

// 1 byte reception function
unsigned char uart_recv_byte(void)
{
    // Wait for data to be received
    while (!(UCSR1A & (1 << RXC1)));

    return UDR1;
}

// String reception function
unsigned char uart_recv_string(unsigned char *str)
{
    unsigned char len = 0;
    unsigned char byte;

    for (;;)
    {
        if (len >= UART_RX_BUFLEN - 1)	// limit character size
	    break;
	
	byte = uart_recv_byte();

	if (byte == CR) // Carriage Return (Enter key)
	{
	    uart_send_byte('\n');
	    uart_send_byte('\r');

	    break;
	}
	else if (byte == BS) // Back Space
	{
	    if (len)
	        len--;
	    
	    uart_send_byte(byte);
	    uart_send_byte(' ');
	    uart_send_byte(byte);
	}
	else
	{
	    str[len++] = byte;

	    uart_send_byte(byte);
	}
    }

    str[len] = '\0';

    return len;
}

// MMI Menu function
void menu_display(void)
{
    uart_send_string("1. LED test\n\r", 13);
    uart_send_string("2. KEY test\n\r", 13);
    uart_send_string("Input number(1-2): ", 19);
}

// GPIO Init
void init_gpio()
{
    DDR_LED |= (1 << PORT7);
    PORT_LED = (1 << PORT7);

    DDR_KEY |= (0 << PORT5);
}

// Timer/Counter Init : Overflow
void init_timer()
{
    // Waveform Generation Mode : Normal mode, Clock : clk/64 (prescaler)
    TCCR0B = (0 << WGM02) | (0 << CS02) | (1 << CS01) | (1 << CS00) ;
    TCCR0A =             (0 << WGM01) | (0 << WGM00);
    // 256 - n
    TCNT0 = 256 - (CPU_CLOCK / TICKS_PER_SEC / 64);    // 64 : prescaler
    // Timer/Counter0 Overflow Interrupt : Enable
    TIMSK0 = (0 << OCIE0B) | (0 << OCIE0A) | (1 << TOIE0);
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


int main(void)
{
    unsigned char buf[UART_RX_BUFLEN];
    unsigned int i;

    // ---- GPIO ----
    init_gpio();

    // ---- Timer/Counter0 ----
    init_timer();

    // --- UART ----
    init_uart();

    // ---- Global Interrupt : Enable ----
    sei();

    for (;;)
    {
        menu_display();
	uart_recv_string(buf);

	switch (atoi(buf))
	{
	    case 1:
	        for (i = 0 ; i < 5 ; i++)
		{
		    PORT_LED = ~(1 << PORT7);
		    delay_ms(200);
		    PORT_LED = (1 << PORT7);
		    delay_ms(200);
		}

		uart_send_string("OK.\r\n", 5);
		break;

	    case 2:
                for (;;)
		{
		    if (!(PIN_KEY & 0x20))
		    {
		        PORT_LED = ~(1 << PORT7);
			delay_ms(1000);
			PORT_LED = (1 << PORT7);

			break;
		    }
		}
		
		uart_send_string("OK.\r\n", 5);
		break;

	    default:
	        uart_send_string("Invalid Arguments!\r\n", 20);
		break;
	}
    }
}

