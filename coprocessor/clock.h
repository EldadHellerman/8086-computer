#ifndef CLOCK_H_
#define CLOCK_H_

#include <stdint.h>

/** Calls clock_config() with appropriate values */
#define CLOCK_CONFIG(CLOCK)     (clock_config(CLOCK##_TOP, CLOCK##_THRESHOLD, CLOCK##_SOURCE));

// Different values for different clock rates, all are ~33% duty cycle:
#define CLOCK_60_HZ
#define CLOCK_60_HZ_TOP             255
#define CLOCK_60_HZ_THRESHOLD       84
#define CLOCK_60_HZ_SOURCE          CLOCK_SOURCE_PRESCALER_1024
#define CLOCK_100_HZ
#define CLOCK_100_HZ_TOP            155
#define CLOCK_100_HZ_THRESHOLD      51
#define CLOCK_100_HZ_SOURCE         CLOCK_SOURCE_PRESCALER_1024
#define CLOCK_100k_HZ
#define CLOCK_100k_HZ_TOP           159
#define CLOCK_100k_HZ_THRESHOLD     52
#define CLOCK_100k_HZ_SOURCE        CLOCK_SOURCE_PRESCALER_1
#define CLOCK_1M_HZ
#define CLOCK_1M_HZ_TOP             15
#define CLOCK_1M_HZ_THRESHOLD       4
#define CLOCK_1M_HZ_SOURCE          CLOCK_SOURCE_PRESCALER_1
#define CLOCK_2M6_HZ
#define CLOCK_2M6_HZ_TOP            5
#define CLOCK_2M6_HZ_THRESHOLD      1
#define CLOCK_2M6_HZ_SOURCE         CLOCK_SOURCE_PRESCALER_1
#define CLOCK_5M3_HZ
#define CLOCK_5M3_HZ_TOP            2
#define CLOCK_5M3_HZ_THRESHOLD      0
#define CLOCK_5M3_HZ_SOURCE         CLOCK_SOURCE_PRESCALER_1


// clock sources:
#define CLOCK_SOURCE_HALT                 0 /** Halt clock */
#define CLOCK_SOURCE_PRESCALER_1          1 /** Use prescaler of 1 */
#define CLOCK_SOURCE_PRESCALER_8          2 /** Use prescaler of 8 */
#define CLOCK_SOURCE_PRESCALER_64         3 /** Use prescaler of 64 */
#define CLOCK_SOURCE_PRESCALER_256        4 /** Use prescaler of 256 */
#define CLOCK_SOURCE_PRESCALER_1024       5 /** Use prescaler of 1024 */

/**
 * @brief Configure timer 0 to output clock signal.
 * 
 * @param top Max counter value.
 * @param threshold Value to output low when reached.
 * @param clock_source Clock source, can be CLOCK_SOURCE_HALT which will stop the timer,
 *      or any of the prescaler values which are defined as CLOCK_SOURCE_XXX,
 *      which will start the timer, which in turn will generate the clock signal.
 * 
 * @note Values begin from 0, so normally 1 should be subtracted from the desired value.
 * For example - top = 255 will output a frequency equal to (timer_clk / 256).
 * 
 */
void clock_config(uint8_t top, uint8_t threshold, uint8_t clock_source);

/**
 * @brief Stops the clock.
 * 
 * @note Same as calling clock_config(x, x, CLOCK_SOURCE_HALT).
 */
void clock_stop();

#endif /* CLOCK_H_ */