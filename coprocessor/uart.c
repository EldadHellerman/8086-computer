#include "uart.h"

#include <avr/common.h> //for SREG definition
#include <avr/interrupt.h>

// set up uart_stream with put_char and get_char functions
static FILE uart_stream = FDEV_SETUP_STREAM(uart_putchar, uart_getchar, _FDEV_SETUP_RW);

/** @brief see header file. */
void uart_config(uint16_t UBRR_value, bool double_speed){
	/*
	* For exact data about uart and register description, see AtMega328p datasheet chapter 19 - USART0.
 	* https://ww1.microchip.com/downloads/en/DeviceDoc/Atmel-7810-Automotive-Microcontrollers-ATmega328P_Datasheet.pdf#page=143
 	*/
	stdout = &uart_stream;
	stdin = &uart_stream;

	UBRR0 = UBRR_value;
	if(double_speed) UCSR0A |= (1 << U2X0);
	// enable transmitter, receiver and receiver interrupts:
	UCSR0B = (1 << TXEN0) | (1 << RXEN0) | (1 << RXCIE0);
	// 8-bit character size:
	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
	
}

/** @brief see header file. */
int uart_putchar(char c, FILE *stream){
	loop_until_bit_is_set(UCSR0A, UDRE0); // check that data register is empty
	UDR0 = c;
	return(0);
}

/** @brief see header file. */
int uart_getchar(){
	loop_until_bit_is_set(UCSR0A, RXC0); // wait for data to be received
	return((int)UDR0);
}