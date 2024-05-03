# Coprocessor

The coprocessor is used for:
 - Generating a clock for the 8086 - The clock is required to be 33% duty cycle.
 - Generating reset signal for the 8086 - Reset is needed for a minimum clock cycles number.
 - Bridging between USB and the 8086 - It uses UART and shows as a COM port when connected to USB.
 - Reading and writing to memory - Used to upload programs, dump memory, control IO, and so on.
 - Generate interrupts - Reguler interrupts (256 different types) and NMI can be generated.
 - Test signal - Control over 8086 Test signal.
 - OPTIONAL: different peripherals:
    The coprocessor has part of the address space and can be used as different peripherals for the 8086.
    Either via direct memory r/w instructions (if AVR is fast enough), or via shared memory in RAM.
    The peripherals may be:
    - UART - using coprocessor's UART.
    - Timers - using coprocessor's timers.
    - DMA
    - FPU
    - Touch screen - ADC6 and ADC7 are available for reading,
      for driving, READY and TEST are available, and on bus hold, INTA and RCLK are available.
    - SD card - maybe common SPI lines, and use 8086 ???? as SD card CS.
