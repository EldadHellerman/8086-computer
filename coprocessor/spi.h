#ifndef SPI_H_
#define SPI_H_

#include <avr/io.h>
#include "gpio.h"

#define SPI_SET_CPHA_TO_0()   (SPCR &= ~(1 << CPHA))
#define SPI_SET_CPHA_TO_1()   (SPCR |= (1 << CPHA))

#define SPI_CLOCK_RATE_PRESCALER_4             0
#define SPI_CLOCK_RATE_PRESCALER_16            1
#define SPI_CLOCK_RATE_PRESCALER_64            2
#define SPI_CLOCK_RATE_PRESCALER_128           3
#define SPI_CLOCK_RATE_DOUBLE_PRESCALER_2      4
#define SPI_CLOCK_RATE_DOUBLE_PRESCALER_8      5
#define SPI_CLOCK_RATE_DOUBLE_PRESCALER_32     6
#define SPI_CLOCK_RATE_DOUBLE_PRESCALER_64     7

/**
 * @brief Configure SPI periperal.
 * 
 * @param clock_rate SPI clock rates. SPI_CLOCK_RATE_xxx should be used.
 */
void spi_config(uint8_t clock_rate);

uint16_t spi_recieve();
void spi_send(uint32_t data);

#endif /* SPI_H_ */