#include <stdio.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "clock.h"
#include "uart.h"
#include "isp.h"
#include "bus.h"

#include "spi.h"

#define FIRMWARE_VERSION    0x0100
#ifndef DEVICE_ID
	#error DEVICE_ID must be define externally
#endif

/* TODO list:
 - work on spi.c and spi.h (currently ad_get or ad_set or both are stuck)
 - work on python utility!
*/


/** //////////////
 * @brief 
 * 
 * 8086 interrupt types are:
 * 	0 - divided by 0
 * 	1 - if TF (trap flag) is set - single step execution
 * 	2 - NMI
 * 	3 - INT3, meant for calling the debugger
 * 	4 - INTO (interrupt if OF flag is set)
 * internal interrupts ("INT 123" for example) cannot be turned off.
 * interrupts 0-31 are supposedly 'reserved'
 * 
 * 
 * @param interrupt_number 
 */
void generate_interrupt(uint8_t interrupt_number){
	// M/IO is low during interrupt number reading
	// and since 8086 has ownership of the bus, M/IO must be 0 to prevent a short circuit.
	// also, during interrupt acknowledge, A16-A19 do not float!,
	// so they must be set to 0 to prevent short circuits.
	// TODO check that's true and that there are no shorts caused. maybe they stay the same as the previous address addressed.

	GPIO_OUTPUT_LOW(INTR);  //needed for some reason... shouldn't be as its already called in main... XXX
	GPIO_SET_OUTPUT(INTR); //

	GPIO_SET_INPUT(INTA_N);
	GPIO_OUTPUT_LOW(INTA_N);

	// ad_set(0x00000000 | interrupt_number);
	GPIO_OUTPUT_HIGH(INTR);
	while(GPIO_GET_INPUT(INTA_N));
	GPIO_OUTPUT_LOW(INTR);
	while(!GPIO_GET_INPUT(INTA_N));
	// GPIO_OUTPUT_LOW(REG_OE_N);
	while(GPIO_GET_INPUT(INTA_N));
	GPIO_OUTPUT_LOW(REG_OE_N); // try moving this before previous while
	while(!GPIO_GET_INPUT(INTA_N));
	GPIO_OUTPUT_HIGH(REG_OE_N);
}

void generate_nmi(void){
	GPIO_OUTPUT_LOW(NMI);  //needed for some reason... shouldn't be as its already called in main... XXX
	GPIO_SET_OUTPUT(NMI); //
	GPIO_OUTPUT_HIGH(NMI);
	while(GPIO_GET_INPUT(CLK));

	while(!GPIO_GET_INPUT(CLK));
	while(GPIO_GET_INPUT(CLK)); //NMI should be on for at

	while(!GPIO_GET_INPUT(CLK));
	while(GPIO_GET_INPUT(CLK)); //least two clock cycles
	
	GPIO_OUTPUT_LOW(NMI);
}

// TODO check this:
/*
ISR(PCINT1_vect){
	return;
	uint16_t address, data;
	GPIO_OUTPUT_LOW(READY);
	address = ad_get();
	int t = 0;
	while(t<100){
		if(!GPIO_GET_INPUT(WR_N)){ //write
			//address is not valid!. maybe if i hand insert the LD pulse before any (automatically generated) prologue code
			data = ad_get();
			printf("wr %x\n", data);
			GPIO_OUTPUT_HIGH(READY);
			break;
		}else if(!GPIO_GET_INPUT(RD_N)){ //read
			char *str = "this is a test string message!";
			//can take as much time as i want to...
			if(address >= 30) ad_set(0);
			else ad_set((str[(address & ~1)+1] << 8) | str[(address & ~1)]);
			GPIO_OUTPUT_LOW(REG_OE_N);
			GPIO_OUTPUT_HIGH(READY);
			while(!GPIO_GET_INPUT(RD_N));
			GPIO_OUTPUT_HIGH(REG_OE_N);
			break;
		}
		t++;
	}
}
*/








/**
 * @brief Preforms 8086 reset sequence, and wait for bus hold acknowledgment.
 * 
 * This will immediately hold the bus after reset, to prevent any instruction from executing.
 * See "PROCESSOR RESET AND INITIALIZATION" on page 10 of 8086 datasheet.
 * 
 * @note A clock signal should be present for the reset sequence to work properly,
 * since after reset, it take a few clock cycles for status lines to float to 3-state,
 * and for bus hold to be acknowledged.
 */
void reset_sequence(void){
	// request hold and reset 8086:
	GPIO_OUTPUT_HIGH(RESET);
	GPIO_OUTPUT_HIGH(HOLD);
	_delay_ms(200); // this depends on the clock speed. 200ms should be good down to about 50Hz clock.
	// on a fast clock, 20us should be enough.
	GPIO_OUTPUT_LOW(RESET);
	// wait for hold acknowledge:
	while(GPIO_GET_INPUT(HLDA) == 0);
}

/**
 * @brief Initalizes all GPIO pins. Leaves RESET as high.
 * 
 * AVR pins are all floating after reset, but 8086 pins are *not* all floating after reset.
 * So after reset, it must be assumed that the 8086 is the bus master, to avoid short circuits.
 * 
 */
void init_gpio(void){
	// coprocessor controlled signals:
	GPIO_OUTPUT_HIGH(RESET);
	GPIO_SET_OUTPUT(RESET);

	GPIO_OUTPUT_LOW(ALE);
	GPIO_SET_OUTPUT(ALE);

	GPIO_OUTPUT_LOW(TEST_N);
	GPIO_SET_OUTPUT(TEST_N);

	GPIO_OUTPUT_LOW(INTR);
	GPIO_SET_OUTPUT(INTR);

	GPIO_OUTPUT_LOW(NMI);
	GPIO_SET_OUTPUT(NMI);

	GPIO_OUTPUT_LOW(HOLD);
	GPIO_SET_OUTPUT(HOLD);
	
	GPIO_SET_INPUT(HLDA);
	GPIO_OUTPUT_LOW(HLDA);

	// it is unclear from datasheet and user family manual if INTA floats during bus hold or not.
	// since the coprocessor is the only one to read this signal, it doesn't matter.
	GPIO_SET_INPUT(INTA_N);
	GPIO_OUTPUT_HIGH(INTA_N);
	GPIO_SET_OUTPUT(INTA_N);

	// coprocessor controlled signals, only if its chip select is enabled:
	GPIO_SET_INPUT(READY);
	
	// bus master controlled signals:
	// all bus master lines will start as inputs. only on bus hold will they change to outputs.
	GPIO_SET_INPUT(RD_N);
	GPIO_SET_INPUT(WR_N);
	// M_IO and BHE also float, but they are controlled via the address shift registers.
}

int main(void){
	init_gpio(); // reset is high after this.
	UART_CONFIG(UART_1M_16MHZ);
	uart_enable_rx_interrupt();
	spi_config(SPI_CLOCK_RATE_PRESCALER_64); // spi_config(SPI_CLOCK_RATE_PRESCALER_2);
	sei();

	CLOCK_CONFIG(CLOCK_100k_HZ);
	reset_sequence();
	_delay_ms(100);

	printf("program started!!\n");

	// /*
	for(int i = 0; i < 5; i++){
		bus_release();
		_delay_us(20);
		bus_hold();
		_delay_ms(2000);
	}
	// */

	// /*
	cli();
	for(int t = 0; t < 20; t++){
		GPIO_OUTPUT_LOW(REG_OE_N);
		for(int i=0; i < 16; i++){
			printf("%d, ", i);
			ad_set((((uint32_t)i) << 16));
			GPIO_OUTPUT_HIGH(ALE);
			asm("NOP\r\n NOP\r\n NOP");
			GPIO_OUTPUT_LOW(ALE);
			_delay_ms(200);
		}
		for(int i=0; i<16; i++){
			printf("%d, ", i);
			ad_set((((uint32_t)i) << 16));
			GPIO_OUTPUT_HIGH(ALE);
			asm("NOP\r\n NOP\r\n NOP");
			GPIO_OUTPUT_LOW(ALE);
			_delay_ms(100);
		}
		GPIO_OUTPUT_HIGH(REG_OE_N);
	}
	// */
	while(1); // only responding to uart commands via interrupts
}


uint8_t isp_input_buffer[ISP_PACKET_MAX_PACKET_LENGTH];
uint16_t isp_input_buffer_index = 0;
uint16_t isp_input_packet_size = 0;

uint32_t parameter_write_address;
uint32_t parameter_clock_speed;
uint32_t parameter_uart_speed;
uint32_t parameter_reset_time;
uint32_t parameter_timeout;
uint32_t parameter_gpio_test;

/** //////////////////
 * @brief 
 * 
 * @param packet_id 
 * @param data_length 0-255
 * @param data can be NULL if data length is 0
 */
void responde(uint8_t packet_id, uint8_t data_length, uint8_t *data){
	uart_putchar(packet_id, NULL);
	uart_putchar(data_length, NULL);
	for(uint8_t i = 0; i < data_length; i++) uart_putchar(data[i], NULL);
}

void responde_0_is_256(uint8_t packet_id, uint8_t data_length, uint8_t *data){
	uart_putchar(packet_id, NULL);
	uart_putchar(data_length, NULL);
	uint8_t i = 0;
	do{
		uart_putchar(data[i++], NULL);
	}while(i != data_length);
}


void command_parameter_read(uint8_t *packet_data){
	uint8_t parameter = *packet_data;
	uint32_t data;
	switch(parameter){ // parameter_id to read
		case PARAMETER_DEVICE_ID:
			data = DEVICE_ID;
			break;
		case PARAMETER_FIRMWARE_VERSION:
			data = FIRMWARE_VERSION;
			break;
		case PARAMETER_PROTOCOL_VERSION:
			data = ISP_PROTOCOL_VERSION;
			break;
		case PARAMETER_WRITE_ADDRESS:
			data = parameter_write_address;
			break;
		case PARAMETER_CLOCK_SPEED:
			data = parameter_clock_speed;
			break;
		case PARAMETER_UART_SPEED:
			data = parameter_uart_speed;
			break;
		case PARAMETER_RESET_TIME:
			data = parameter_reset_time;
			break;
		case PARAMETER_TIMEOUT:
			data = parameter_timeout;
			break;
		case PARAMETER_GPIO_TEST:
			data = parameter_gpio_test;
			break;
		default:
			responde(INVALID_PARAMETER, 0, NULL);
			return;
	}
	responde(SUCCESS_READ_PARAMETER, 4, (uint8_t *)(&data));
}

void command_parameter_write(uint8_t *packet_data){
	uint8_t parameter = *packet_data;
	packet_data++;
	uint32_t data = *((uint32_t *)packet_data);
	switch(parameter){ // parameter_id to read
		case PARAMETER_DEVICE_ID:
			responde(FAILURE, 0, NULL);
			break;
		case PARAMETER_FIRMWARE_VERSION:
			responde(FAILURE, 0, NULL);
			break;
		case PARAMETER_WRITE_ADDRESS:
			parameter_write_address = data;
			break;
		case PARAMETER_CLOCK_SPEED:
			parameter_clock_speed = data;
			break;
		case PARAMETER_UART_SPEED:
			parameter_uart_speed = data;
			break;
		case PARAMETER_RESET_TIME:
			parameter_reset_time = data;
			break;
		case PARAMETER_TIMEOUT:
			parameter_timeout = data;
			break;
		case PARAMETER_GPIO_TEST:
			parameter_gpio_test = data;
			break;
		default:
			responde(INVALID_PARAMETER, 0, NULL);
			return;
	}
	responde(SUCCESS, 0, NULL);
}

void command_bus_read(uint8_t *packet_data){
	uint32_t address = *((uint32_t *)packet_data);
	packet_data += 4;
	uint16_t length = *packet_data;
	if(length == 0) length = 256; // 0 is 256
	uint8_t buffer[length];
	// printf("bus read of address %ld\n", address);
	uint8_t status = bus_read(address, length, buffer);
	// printf("left command bus read!");
	if(status == 0){
		responde_0_is_256(SUCCESS_READ, (uint8_t)length, buffer); // length converted from 256 back to 0
	}else{
		responde(FAILURE, 0, NULL);
	}
}

void command_bus_write(uint8_t *packet_data, uint16_t data_length){
	uint8_t status = bus_write(parameter_write_address, data_length, packet_data);
	if(status == 0) responde(SUCCESS, 0, NULL);
	else responde(FAILURE, 0, NULL);
}

void parse_packet(){
	uint8_t packet_id = isp_input_buffer[ISP_FIELD_INDEX_PACKET_ID];
	uint16_t data_length = isp_input_buffer[ISP_FIELD_INDEX_DATA_LENGTH];
	uint8_t *data = &isp_input_buffer[ISP_FIELD_INDEX_DATA_START];
	if((packet_id & ISP_DATA_RANGE_1_TO_256_FLAG) && data_length == 0) data_length = 256;  // 0 is 256 bytes

	switch(packet_id){
		case PARAMETER_READ:
			if(data_length != ISP_DATA_LENGTH_PARAMETER_READ) break;
			command_parameter_read(data);
			return;
		case PARAMETER_WRITE:
			if(data_length != ISP_DATA_LENGTH_PARAMETER_WRITE) break;
			command_parameter_write(data);
			return;
		case RESET_AND_BUS_HOLD:
			if(data_length != ISP_DATA_LENGTH_RESET_AND_HOLD_BUS) break;
			reset_sequence();
			responde(SUCCESS, 0, NULL);
			return;
		case BUS_HOLD:
			if(data_length != ISP_DATA_LENGTH_BUS_HOLD) break;
			bus_hold();
			responde(SUCCESS, 0, NULL);
			return;
		case BUS_RELEASE:
			if(data_length != ISP_DATA_LENGTH_BUS_RELEASE) break;
			bus_release();
			responde(SUCCESS, 0, NULL);
			return;
		case BUS_READ:
			if(data_length != ISP_DATA_LENGTH_BUS_READ) break;
			command_bus_read(data);
			return;
		case GENERATE_INTERRUPT:
			if(data_length != ISP_DATA_LENGTH_GENERATE_INTERRPUT) break;
			generate_interrupt(*data);
			responde(SUCCESS, 0, NULL);
			return;
		case GENERATE_NMI:
			if(data_length != ISP_DATA_LENGTH_GENERATE_NMI) break;
			generate_nmi();
			responde(SUCCESS, 0, NULL);
			return;
		case BUS_WRITE:
			command_bus_write(data, data_length);
			return;
		default:
			responde(INVALID_COMMAND, 0, NULL);
	}
	// if reached here, that mean packet has invalid length
	responde(INVALID_DATA_LENGTH, 0, NULL);
}

ISR(USART_RX_vect){
	uint8_t data = UDR0;
	if(isp_input_buffer_index == ISP_FIELD_INDEX_DATA_LENGTH){
		isp_input_packet_size = ISP_PACKET_HEADER_LENGTH + ISP_PACKET_FOOTER_LENGTH + data;
	}
	isp_input_buffer[isp_input_buffer_index++] = data;

	if(isp_input_buffer_index == isp_input_packet_size){ // entire packet was read
		parse_packet(); // parse received packet
		isp_input_buffer_index = 0;
		isp_input_packet_size = 0; // get ready to receive new packet
	}
}

