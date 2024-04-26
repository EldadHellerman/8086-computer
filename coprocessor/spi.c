#include "spi.h"

void spi_config(uint8_t clock_rate){
	GPIO_OUTPUT_HIGH(REG_OE_N);
	GPIO_OUTPUT_LOW(RCLK_SH_LD_N);
	
	GPIO_SET_OUTPUT(REG_OE_N);
	GPIO_SET_OUTPUT(RCLK_SH_LD_N);
	GPIO_SET_OUTPUT(SPI_MOSI);
	GPIO_SET_OUTPUT(SPI_SCK);
	GPIO_SET_INPUT(SPI_MOSI);

	// GPIO_SET_INPUT(INTA_N);    // TEST should have been connected to SS
	// GPIO_OUTPUT_HIGH(INTA_N);  // because it is always output.

	SPCR = (1 << SPE) | (1 << MSTR); // enable SPI as master
	SPCR |= (1 << DORD); // transmit LSB of data first
	SPCR |= (clock_rate & 0x3) << SPR0; // set SPR[1:0] bits to select clock rate
	SPSR |= (clock_rate & 0x4) << SPI2X; // set SPI2X for double SPI speed

	// 74hc595 shifts in values on rising clok edge,
	// 74hc165 shifts out value on rising clock edge - hence it needs to be read on falling edge
	// thats why for sending data - CPHA should be 0, and for reading data - CPHA should be 1
}

/**
 * @brief Shifts out a byte and returns the byte received.
 * 
 * @param data Data to shift out.
 * @return uint8_t Data shifted in.
 */
uint8_t spi_transaction(uint8_t data){
	SPSR &= ~(1 << SPIF);
	// GPIO_OUTPUT_HIGH(INTA_N);  // because it is always output.
	asm("NOP\r\n NOP\r\n NOP");
	SPDR = data; // write data
	// printf("#A,");
	// printf("SPCR: 0x%02X, SPSR: 0x%02X,", SPCR, SPSR);
	loop_until_bit_is_set(SPSR, SPIF); // wait for shift to complete
	SPSR &= ~(1 << SPIF);
	// printf("#B,");
	return SPDR; // return data shifted in
}

void spi_send(uint32_t data){
	SPI_SET_CPHA_TO_0();
	spi_transaction(data >> 0);
	spi_transaction(data >> 8);
	spi_transaction(data >> 16);
}

uint16_t spi_recieve(){
	SPI_SET_CPHA_TO_1();
	uint16_t result = spi_transaction(0);
	result |= (spi_transaction(0) << 8);
	return(result);
}

uint16_t spi_recieve_2(){
	uint16_t result = spi_transaction(0);
	result |= (spi_transaction(0) << 8);
	return(result);
}