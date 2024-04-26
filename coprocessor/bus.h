#ifndef BUS_H_
#define BUS_H_

#include <inttypes.h>

// TODO check for timeout and return error if cant hold or release bus after timeout
void bus_hold(void);
void bus_release(void);

uint8_t bus_read(uint32_t address, uint16_t data_length, uint8_t *data);
uint8_t bus_write(uint32_t address, uint16_t data_length, uint8_t *data);

#endif /* BUS_H_ */