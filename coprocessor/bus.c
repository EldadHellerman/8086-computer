#include "bus.h"
#include "gpio.h"
#include "spi.h"

void bus_hold(void){
    // RD and WR are pulled high by external pullup resistors.

    // request hold and wait for aknowledge:
	GPIO_OUTPUT_HIGH(HOLD);
	while(!GPIO_GET_INPUT(HLDA));
    
    // output high on RD and WR  and change them to output
    GPIO_OUTPUT_HIGH(RD_N);
    GPIO_OUTPUT_HIGH(WR_N);
	GPIO_SET_OUTPUT(RD_N);
	GPIO_SET_OUTPUT(WR_N);
}

void bus_release(void){
	// TODO: prevent another hold request from happenning to quickly. check 8086 family user manual.
    // RD and WR are pulled high by external pullup resistors.

    // set RD and WR to inputs:
    GPIO_SET_INPUT(RD_N);
    GPIO_SET_INPUT(WR_N);
	GPIO_OUTPUT_LOW(RD_N);
	GPIO_OUTPUT_LOW(WR_N); 

    // let go of hold, and wait for 8086 to grab the bus back.
	GPIO_OUTPUT_LOW(HOLD);
	while(GPIO_GET_INPUT(HLDA));
}

void ad_set(uint32_t address){
	spi_send(address);
	GPIO_OUTPUT_HIGH(RCLK_SH_LD_N);
	asm("NOP\r\n NOP\r\n NOP\r\n");
	GPIO_OUTPUT_LOW(RCLK_SH_LD_N);
}

uint16_t ad_get(void){
	// doesnt work if LD pulse is before CPOL change, why?
	SPI_SET_CPHA_TO_1();
	asm("NOP\r\n NOP\r\n NOP\r\n");
	GPIO_OUTPUT_HIGH(RCLK_SH_LD_N);
	asm("NOP\r\n NOP\r\n NOP\r\n");
	GPIO_OUTPUT_LOW(RCLK_SH_LD_N); // return back to low, for rclk.
	// uint16_t r = spi_recieve();
	uint16_t r = spi_recieve_2();
	return(r);
}



uint8_t bus_read(uint32_t address,  uint16_t data_length, uint8_t *data){
	for(uint16_t i = 0; i < data_length; i += 2){
		// printf("#0,");
		ad_set(address & (~BHE_IN_ADDRESS)); // make sure that BHE is 0
		// printf("#1,");
		GPIO_OUTPUT_LOW(REG_OE_N); asm("NOP\r\n NOP\r\n NOP\r\n");
		GPIO_OUTPUT_HIGH(ALE); asm("NOP\r\n NOP\r\n NOP\r\n");
		GPIO_OUTPUT_LOW(ALE); asm("NOP\r\n NOP\r\n NOP\r\n");
		GPIO_OUTPUT_HIGH(REG_OE_N); asm("NOP\r\n NOP\r\n NOP\r\n");
		GPIO_OUTPUT_LOW(RD_N); asm("NOP\r\n NOP\r\n NOP\r\n");
		// printf("#2,");
		uint16_t r = ad_get();
		// printf("#3,");
		GPIO_OUTPUT_HIGH(RD_N);
		data[i] = (uint8_t)r;
		data[i + 1] = (uint8_t)(r>>8);
	}
	return 0;
}

uint8_t bus_write(uint32_t address, uint16_t data_length, uint8_t *data){
	// printf("address %ld ", address);
	for(uint16_t i = 0; i < data_length; i += 2){
		ad_set(address & (~BHE_IN_ADDRESS)); // make sure that BHE is 0
		GPIO_OUTPUT_LOW(REG_OE_N);
		GPIO_OUTPUT_HIGH(ALE);
		GPIO_OUTPUT_LOW(ALE);
		// printf("w %02X-%02X ", data[i], data[i+1]);
		ad_set((data[i] << 8) | data[i+1]);
		GPIO_OUTPUT_LOW(WR_N);
		GPIO_OUTPUT_HIGH(WR_N);
		GPIO_OUTPUT_HIGH(REG_OE_N);
	}
	return 0;
}

/*
static spi_init();
static spi_send();

static ad_get();
static ad_set();
*/






/*

void program_my_program(void){
	flash_prog(0xffff0, 0x00BC); flash_prog(0xffff2, 0xEA10); flash_prog(0xffff4, 0x0020); flash_prog(0xffff6, 0x0000);
	flash_prog(0xffff8, 0x9090); flash_prog(0xffffa, 0x9090); flash_prog(0xffffc, 0x9090); flash_prog(0xffffe, 0x9090);
	write_word(0x80000,0x00ff);
}

void erase_flash(void){
	uint32_t address = get_uint32();
	if(!(address & 0x80000)) return;
	address = (address & ~0x80000) >> 1;
	flash_block_erase(address);
}


void previous_uart_rx_isr(void){
	switch(UDR0){
		case '1':
			flash_block_erase(0x3E000);
			flash_read_mode();
			printf("done 1!\n");
			program_my_program();
			printf("done 2!\n");
			break;
		case '2':
			flash_block_erase(0x00000);
			flash_block_erase(0x10000);
			flash_block_erase(0x20000);
			flash_block_erase(0x30000);
			flash_block_erase(0x3C000);
			flash_block_erase(0x3D000);
			flash_block_erase(0x3E000);
			break;
	}
}

void upload_to_memory(void){
	uint32_t start_address = get_uint32();
	uint32_t length = get_uint32() & ~1;
	uint32_t end = start_address+length;
	int buffer_pointer;
	
	// for(uint32_t i=start_address; i< start_address + (length&0xff00); i+=256){
	// 	//program in 256 blocks
	// }
	// for(uint32_t i=start_address + (length&0xff00); i< start_address + length; i+=256){
	// 	//program whats left
	// }
	
	for(uint32_t i=start_address; i<end; i+=256){
		_Bool flag = length < 256;
		buffer_pointer = 0;
		if(flag){ do buffer[buffer_pointer++] = uart_getchar(); while(buffer_pointer != length);}
		else{ do buffer[buffer_pointer++] = uart_getchar(); while(buffer_pointer != 256);}
		buffer_pointer = 0;
		if(flag){
			do{
				uint16_t word = buffer[buffer_pointer] | (buffer[buffer_pointer+1]<<8);
				//printf("writing %04X to %04X%04X",word,(uint16_t)((i+buffer_pointer)>>16),(uint16_t)(i+buffer_pointer));
				if(i & 0x80000) flash_prog(i+buffer_pointer,word); else write_word(i+buffer_pointer,word);
				buffer_pointer += 2;
			}while(buffer_pointer != length);
		}else{
			do{
				uint16_t word = buffer[buffer_pointer] | (buffer[buffer_pointer+1]<<8);
				if(i & 0x80000) flash_prog(i+buffer_pointer,word); else write_word(i+buffer_pointer,word);
				buffer_pointer += 2;
			}while(buffer_pointer != 256);
		}
		length -= 256;
		uart_putchar(6,NULL);
	}
	write_word(0x80000,0x00ff);
}




char buffer[256];



uint16_t read_word(uint32_t address){
	address &= ~1;						////////////////////////////////////////TODO REMOVE
	uint16_t result;
	ad_set(address & (~BHE_IN_ADDRESS)); //make sure that BHE is 0
	//_delay_us(1);
	GPIO_OUTPUT_LOW(REG_OE_N);
	GPIO_OUTPUT_HIGH(ALE);
	GPIO_OUTPUT_LOW(ALE);
	GPIO_OUTPUT_HIGH(REG_OE_N);
	GPIO_OUTPUT_LOW(RD_N);
	result = ad_get();
	GPIO_OUTPUT_HIGH(RD_N);
	return(result);
}

uint8_t read_byte(uint32_t address){
	//ad_set(0); ////////////////////////////////////////////////////////////TODO REMOVE
	uint16_t word = read_word(address);
	return((address & 1) ? (word>>8) : (word&0xff)); //XXX XXX XXX/////////////////////////////////////////////////////is this working?
}

void write_word(uint32_t address, uint16_t data){
	address &= ~1;						////////////////////////////////////////TODO REMOVE
	ad_set(address & (~BHE_IN_ADDRESS)); //make sure that BHE is 0
	GPIO_OUTPUT_LOW(REG_OE_N);
	GPIO_OUTPUT_HIGH(ALE);
	GPIO_OUTPUT_LOW(ALE);
	ad_set(0x00000000 | data);
	GPIO_OUTPUT_LOW(WR_N);
	GPIO_OUTPUT_HIGH(WR_N);
	GPIO_OUTPUT_HIGH(REG_OE_N);
}

void write_byte(uint32_t address, uint8_t data){
	uint16_t word = read_word(address&(~1));
	if(address & 1) word = (word & 0x00ff) | (((uint16_t)data)<<8);
	else word = (word & 0xff00) | data;
	write_word(address&(~1),word);
}

void flash_read_mode(void){
	//flash_read_status_register();
	//flash_clear_status_register();
	write_word(0x80000,0x00ff);
}

uint8_t flash_read_status_register(void){
	write_word(0x80000,0x0070);
	return(read_word(0x80000) & 0xff);
}

void flash_clear_status_register(void){
	write_word(0x80000,0x0050);
}

void flash_block_erase(uint32_t block_word_address){
	write_word(0x80000 | (block_word_address<<1), 0x0020);
	write_word(0x80000 | (block_word_address<<1), 0x00D0);
	uint8_t status;
	while(1){
		//status = (read_word(0x80000) & 0xff); 
		status = flash_read_status_register();
		if(status & (1<<7)) break;
	}
	if(status & (1<<3)) printf("Vpp error\n");
	if((status & (1<<4)) && (status & (1<<5))) printf("CSE error\n");
	if(status & (1<<5)) printf("BLE error\n");
	write_word(0x80000,0x00ff);
}

void flash_write_word(uint32_t address, uint16_t data){
	write_word(address, 0x0040);
	write_word(address, data);
}

void flash_prog(uint32_t address, uint16_t data){
	flash_write_word(0x80000 | address, data);
	uint8_t status;
	while(1){
		status = flash_read_status_register();
		if(status & (1<<7)) break;
	}
	if(status & (1<<3)) printf("Vpp error\n");
	if(status & (1<<4)) printf("prg error\n");
}

*/