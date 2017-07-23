/*
********************************************************************************
* File        	: EX4_1.C
* By          	: FunFun Yoo
*
* Description 	: Example for UART (ASCII Echo)
*
* Copyright(c) 2017 DrimAES Inc. All rights are reserved.
*
*
* 2017.01.17	: Last modified
*
********************************************************************************
*/

#include <avr/io.h>

#define CPU_CLOCK	16000000

#define BAUD_RATE	19200
#define BAUD_RATE_L	(CPU_CLOCK / (16l * BAUD_RATE)) - 1
#define BAUD_RATE_H	((CPU_CLOCK / (16l * BAUD_RATE)) - 1) >> 8

#define BS		0x08	// ASCII : Back Space
#define CR		0x0D	// ASCII : Carriage Return (Enter key)


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
    unsigned char byte;

    // --- UART ----
    init_uart();

    for (;;)
    {
        byte = uart_recv_byte();

	switch (byte)
	{
	    case CR: // Carriage Return (Enter key)
	        uart_send_byte('\n');
		uart_send_byte('\r');
		break;
	
	    case BS: // Back Space
	        uart_send_byte(byte);
		uart_send_byte(' ');
		uart_send_byte(byte);
		break;
	
	    default:
	        uart_send_byte(byte);
		break;
	}
    }
}

