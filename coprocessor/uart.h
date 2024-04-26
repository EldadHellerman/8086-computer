#ifndef UART_H_
#define UART_H_

#include "stdio.h"
#include "stdbool.h"

/** Calls uart_config() with appropriate values */
#define UART_CONFIG(BAUD_RATE) (uart_config(BAUD_RATE##_UBRR, BAUD_RATE##_DOUBLE))

// Different values for different baud rates and clock speeds:
// see table 19-9 in datasheet
#define UART_9600_16MHZ
#define UART_9600_16MHZ_UBRR         103
#define UART_9600_16MHZ_DOUBLE       false
#define UART_115200_16MHZ
#define UART_115200_16MHZ_UBRR       16
#define UART_115200_16MHZ_DOUBLE     true
#define UART_250k_16MHZ
#define UART_250k_16MHZ_UBRR         3
#define UART_250k_16MHZ_DOUBLE       false
#define UART_500k_16MHZ
#define UART_500k_16MHZ_UBRR         1
#define UART_500k_16MHZ_DOUBLE       false
#define UART_1M_16MHZ
#define UART_1M_16MHZ_UBRR           0
#define UART_1M_16MHZ_DOUBLE         false
#define UART_2M_16MHZ
#define UART_2M_16MHZ_UBRR           0
#define UART_2M_16MHZ_DOUBLE         true

/**
 * @brief Configure UART peripheral, and set it up as stdout and stdin.
 * 
 * Baud rate will be determined as follows:
 *     normal mode -        baud_rate = Fosc / (16 * ( @p UBRR_value + 1))
 *     double speed mode -  baud_rate = Fosc / (8 * ( @p UBRR_value + 1))
 * @p UBRR_value can be calculated as follows:
 *      normal mode -       @p UBRR_value = (Fosc / (16 * baud_rate)) - 1
 *      double speed mode - @p UBRR_value = (Fosc / (8 * baud_rate)) - 1
 * 
 * For more information, see datasheet Table 19.1
 * 
 * @param UBRR_value UBRR register value.
 * @param double_speed Double speed mode or normal mode.
 */
void uart_config(uint16_t UBRR_value, bool double_speed);

/**
 * @brief Write a char to UART.
 * 
 * @param c Data to write.
 * @param stream Don't care. Not used.
 * @return int Returns 0.
 */
int uart_putchar(char c, FILE *stream);

/**
 * @brief Read a char from UART.
 * 
 * @return int data read. A char casted to an int.
 */
int uart_getchar(void);

/**
 * @brief Enable UART RX interrupt.
 */
void uart_enable_rx_interrupt(void);

#endif /* UART_H__H_ */