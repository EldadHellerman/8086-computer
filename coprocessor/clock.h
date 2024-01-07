#ifndef CLOCK_H_
#define CLOCK_H_

#include <stdint.h>

#define CS_HALT                 0
#define CS_PRESCALER_1          1  
#define CS_PRESCALER_8          2
#define CS_PRESCALER_64         3
#define CS_PRESCALER_256        4
#define CS_PRESCALER_1024       5

/**
 * @brief Sets up timer 0 to output clock signal.
 * 
 * @param top Max counter value.
 * @param threshold Value to output low when reached.
 * @param clock_source Clock source, can be CS_HALT which will stop the timer,
 *      or any of the prescaler values which are defined as CS_XXX,
 *      which will start the timer, which in turn will generate the clock signal.
 * 
 * @note Values begin from 0, so normally 1 should be subtracted from the desired value.
 * For example - top = 255 will output a frequency equal to (timer_clk / 256).
 * 
 */
void clock_setup(uint8_t top, uint8_t threshold, uint8_t clock_source);

/**
 * @brief Stops the clock.
 * 
 * @note Same as calling clock_setup(x, x, CS_HALT).
 */
void clock_stop();

#endif /* CLOCK_H_ */