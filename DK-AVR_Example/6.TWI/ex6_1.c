/*
********************************************************************************
* File        	: EX6_1.C
* By          	: FunFun Yoo
*
* Description 	: Example for TWI (Atmel EEPROM : I2C)
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
#include <stdio.h>	// sprintf()

#define CPU_CLOCK	16000000
#define TICKS_PER_SEC	1000

#define BAUD_RATE	19200
#define BAUD_RATE_L	(CPU_CLOCK / (16l * BAUD_RATE)) - 1
#define BAUD_RATE_H	((CPU_CLOCK / (16l * BAUD_RATE)) - 1) >> 8

#define BS		0x08	// ASCII : Back Space
#define CR		0x0D	// ASCII : Carriage Return (Enter key)

#define UART_RX_BUFLEN	128

#define TWI_FREQ	400000
#define AT24CXX_ADDR	0xA0


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
    uart_send_string("1. External EEPROM Initializing\n\r", 33);
    uart_send_string("2. External EEPROM Reading\n\r", 28);
    uart_send_string("3. External EEPROM Writing\n\r", 28);
    uart_send_string("Input number(1-3): ", 19);
}

// AT24CXX : Byte Write 
void twi_write_byte(unsigned int addr, unsigned char byte)
{
    // START
    TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
    while (!(TWCR & (1 << TWINT)));

    // Slave Device Address : 3-bit (10 ~ 8 bit) + Write Command (0x0)
    TWDR = AT24CXX_ADDR | ((addr >> 7) & 0x000E);
    TWCR = (1 << TWINT) | (1 << TWEN);
    while (!(TWCR & (1 << TWINT)));

    // Slave Device Address : 8-bit (7 ~ 0 bit)
    TWDR = addr & 0x00FF;
    TWCR = (1 << TWINT) | (1 << TWEN);
    while (!(TWCR & (1 << TWINT)));

    // Write Data
    TWDR = byte;
    TWCR = (1 << TWINT) | (1 << TWEN);
    while (!(TWCR & (1 << TWINT)));

    // STOP
    TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN);

    // Self-timed Write Cycle (5 ms max)
    delay_ms(5);
}

// AT24CXX : Random Read
unsigned char twi_read_byte(unsigned int addr)
{
    unsigned char byte;

    // START
    TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
    while (!(TWCR & (1 << TWINT)));

    // Slave Device Address : 3-bit (10 ~ 8 bit) + Write Command (0x0)
    TWDR = AT24CXX_ADDR | ((addr >> 7) & 0x000E);
    TWCR = (1 << TWINT) | (1 << TWEN);
    while (!(TWCR & (1 << TWINT)));

    // Slave Device Address : 8-bit (7 ~ 0 bit)
    TWDR = addr & 0x00FF;
    TWCR = (1 << TWINT) | (1 << TWEN);
    while (!(TWCR & (1 << TWINT)));

    // START
    TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
    while (!(TWCR & (1 << TWINT)));

    // Slave Device Address : 3-bit (10 ~ 8 bit) + Read Command (0x1)
    TWDR = AT24CXX_ADDR | ((addr >> 7) & 0x000E) | 0x0001;
    TWCR = (1 << TWINT) | (1 << TWEN);
    while (!(TWCR & (1 << TWINT)));

    // --- Reception Data from AT24CXX ---
    TWCR = (1 << TWINT) | (1 << TWEN);
    while (!(TWCR & (1 << TWINT)));

    byte = TWDR;

    // STOP
    TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN);

    return byte;
}


// Timer/Counter Init
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

// TWI Init
void init_twi()
{
    TWBR = (CPU_CLOCK / TWI_FREQ - 16) / 2;
    TWSR = (0 << TWPS1) | (0 << TWPS0);
}


int main(void)
{
    unsigned char data;
    unsigned char eeprom_data[5];

    unsigned char buf[UART_RX_BUFLEN];
    unsigned int i, len;

    // ---- Timer/Counter0 ----
    init_timer();

    // --- UART ----
    init_uart();

    // ---- TWI ----
    init_twi();

    // ---- Global Interrupt : Enable ----
    sei();

    for (;;)
    {
        menu_display();
	uart_recv_string(buf);

	switch (atoi(buf))
	{
	    case 1:
                // Initialize "0x00" value to address "0x00 ~ 0xFF"
		for (i = 0x00 ; i < 0xFF ; i++)
		    twi_write_byte(i, 0);

		uart_send_string("OK.\r\n", 5);
		break;

	    case 2:
	        // Read value from address "0x00 ~ 0xFF"
		for (i = 0x00 ; i < 0xFF ; i++)
		{
		    data = twi_read_byte(i);

		    // Conversion data into hex type
		    sprintf(eeprom_data, "0x%02X ", data);
		    uart_send_string(eeprom_data, sizeof(eeprom_data));

		    // line display : 15 limit
		    if (((i + 1) % 15) == 0)
		        uart_send_string("\n\r", 2);
		}

		uart_send_string("OK.\r\n", 5);
		break;

            case 3:
	        // Write value from user input 
		len = uart_recv_string(buf);

		for (i = 0 ; i < len ; i++)
		    twi_write_byte(i, buf[i]);

		uart_send_string("OK.\r\n", 5);
		break;

	    default:
	        uart_send_string("Invalid Arguments!\r\n", 20);
		break;
	}
    }
}

