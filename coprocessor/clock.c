#include "clock.h"
#include "gpio.h"

/** @brief see header file. */
void clock_setup(uint8_t top, uint8_t threshold, uint8_t clock_source){
	/*
	* For exact data about timer and register description, see AtMega328p datasheet chapter 14 - 8-bit timer/counter0 with PWM.
 	* https://ww1.microchip.com/downloads/en/DeviceDoc/Atmel-7810-Automotive-Microcontrollers-ATmega328P_Datasheet.pdf#page=84
 	*/
	GPIO_SET_OUTPUT(CLK); // set clock pin as output
	TIMSK0 = 0; 							// no interrupts:
	TCCR0B = (1 << WGM02);				 	// FastPWM mode
	TCCR0A = (1 << WGM01) | (1 << WGM00); 	// FastPWM mode
	TCCR0A |= (1 << COM0B1); 				// non inverting mode - OCRA as TOP, clear on OC0B match and set at BOTTOM (0).
	TCNT0 = 0; 								// set counter to 0.
	OCR0A = top; 							// set OCR0A (top).
	OCR0B = threshold; 						// set OCR0B (clear on match).
	TCCR0B |= (clock_source << CS00);		// set clock source
}

/** @brief see header file. */
void clock_stop(){
	clock_setup(0, 0, CS_HALT);
}