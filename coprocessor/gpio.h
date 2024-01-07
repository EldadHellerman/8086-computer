#ifndef GPIO_H_
#define GPIO_H_

/**
 * @brief Hardware defenition for which pin is what, as well as preprocessor directives for easy GPIO control.
 * 
 * This file maps all signals to their respective ports and pins,
 * enabling statments like GPIO_SET_OUTPUT(ALE) to translate into direct DDRx |= (1 << y).
 * Signal which are active low are marked with a '_N' suffix.
 * Some pins are not remappable and are guaranteed to be on that pin.
 * 
 * Information about proprocessor black magic at https://gcc.gnu.org/onlinedocs/cpp/Argument-Prescan.html.
 */

#include <avr/io.h>

#define XXDDR(X)                (DDR##X)
#define XDDR(X)                 (XXDDR(X))
#define DDR(X) 	                (XDDR(X##_PORT))
#define XXPORT(X)               (PORT##X)
#define XPORT(X)                (XXPORT(X))
#define PORT(X)                 (XPORT(X##_PORT))
#define XXPIN(X)                (PIN##X)
#define XPIN(X)                 (XXPIN(X))
#define PIN(X)                  (XPIN(X##_PORT))
#define BIT(X) 	                (X##_BIT)

/**
 * @brief Sets a signal as output.
 * @param X Signal name.
 */
#define GPIO_SET_OUTPUT(X)	    (DDR(X) 	|=  (1 << BIT(X)))
/**
 * @brief Sets a signal as input.
 * @param X Signal name.
 */
#define GPIO_SET_INPUT(X)	    (DDR(X) 	&= ~(1 << BIT(X)))
/**
 * @brief Sets a signal to output high. If signal is input, it will enable pull-up.
 * @param X Signal name.
 */
#define GPIO_OUTPUT_HIGH(X)		(PORT(X) |=  (1 << BIT(X)))
/**
 * @brief Sets a signal to output low. If signal pin is input, it will disable pull-up.
 * @param X Signal name.
 */
#define GPIO_OUTPUT_LOW(X)		(PORT(X) &= ~(1 << BIT(X)))
/**
 * @brief Reads a signal pin.
 * @param X Signal name.
 * @returns Signal logic level.
 */
#define GPIO_GET_INPUT(X)	    ((PIN(X) &   (1 << BIT(X))))

//////////////////////////////////////////////////
///////////       Signal names:       ////////////
//////////////////////////////////////////////////

// Defined here so that autocomplete will work for them.

#define UART_RX                 UART_RX
#define UART_TX                 UART_TX
#define ALE					    ALE
#define RD_N				    RD_N
#define WR_N				    WR_N
#define CLK					    CLK
#define RESET				    RESET
#define HOLD				    HOLD
#define HLDA				    HLDA
#define TEST_N				    TEST_N
#define READY				    READY
#define NMI					    NMI
#define INTR				    INTR
#define INTA_N				    INTA_N
#define SPI_MOSI			    SPI_MOSI
#define SPI_MISO			    SPI_MISO
#define SPI_SCK				    SPI_SCK
#define REG_OE_N			    REG_OE_N
#define RCLK_SH_LD_N            RCLK_SH_LD_N


//////////////////////////////////////////////////
///////       Signal to pin mapping:       ///////
//////////////////////////////////////////////////

// signals which use pin specific features (like pin interrupt, UART, SPI...):

// SPI:
#define SPI_MOSI_PORT		    B
#define SPI_MOSI_BIT		    3
#define SPI_MISO_PORT		    B
#define SPI_MISO_BIT		    4
#define SPI_SCK_PORT		    B
#define SPI_SCK_BIT			    5

// UART:
#define UART_RX_PORT		    D
#define UART_RX_BIT			    0
#define UART_TX_PORT		    D
#define UART_TX_BIT			    1

// Interrupt 0:
#define MCU_CS_PORT			    D
#define MCU_CS_BIT			    2

// Interrupt 1:
#define ALE_PORT			    D
#define ALE_BIT				    3

// Timer-0 OC0B:
#define CLK_PORT			    D
#define CLK_BIT				    5


// generic signals - can be remapped if neccessary:

#define BHE_IN_ADDRESS          (((uint32_t)1) << 20)
#define M_IO_IN_ADDRESS         (((uint32_t)1) << 22)

#define RCLK_SH_LD_N_PORT	    B
#define RCLK_SH_LD_N_BIT	    0
#define TEST_N_PORT			    B
#define TEST_N_BIT			    1
#define INTA_N_PORT			    B
#define INTA_N_BIT			    2

#define HOLD_PORT			    C
#define HOLD_BIT			    0
#define INTR_PORT			    C
#define INTR_BIT			    1
#define RD_N_PORT 			    C
#define RD_N_BIT			    2
#define NMI_PORT			    C
#define NMI_BIT				    3
#define HLDA_PORT			    C
#define HLDA_BIT			    4
#define WR_N_PORT			    C
#define WR_N_BIT			    5

#define RESET_PORT			    D
#define RESET_BIT			    4
#define REG_OE_N_PORT		    D
#define REG_OE_N_BIT		    6
#define READY_PORT			    D
#define READY_BIT			    7

#endif /* GPIO_H_ */