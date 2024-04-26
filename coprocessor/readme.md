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


## Communication protocol

The coprocessor communicates with a computer using a UART to USB adapter.  

General structure:
- The protocol consists of packets, each being either a command or a response.  
- Only the computer may send commands.
- Only the coprocessor may send responses. 
- Each command is answered with a response.
- Packets have a maximum data payload of 255-256 bytes.
- Parameters are used to set different setting of the coprocessor.  
- Each parameter is identified by a 1-byte id, and consists of up to 4-bytes of data.

Packets are structured like this:
| first byte | second byte | third byte to last byte |
| ---------- | ----------- | ----------------------- |
| packet_id  | data length | data                    |



### Packets table:
| packet_id              | packet type | description                       | data length | (data length) data                                |
| ---------------------- | ----------- | --------------------------------- | :---------: | ------------------------------------------------- |
| PARAMETER_READ         | command     | read a parameter                  | 1           | parameter_id (1 byte)                             |
| PARAMETER_WRITE        | command     | write a parameter                 | 5           | parameter_id (1 byte), parameter_value (4 bytes)  |
| RESET_AND_BUS_HOLD     | command     | reset 8086 and hold the bus       | 0           | none                                              |
| BUS_HOLD               | command     | hold the bus                      | 0           | none                                              |
| BUS_RELEASE            | command     | release the bus                   | 0           | none                                              |
| BUS_READ               | command     | read from bus                     | 5           | address (4 bytes), length (1 byte, 0 is 256)      |
| GENERATE_INTERRUPT     | command     | generate interrupt                | 1           | interrupt_number (1 byte)                         |
| GENERATE_NMI           | command     | generate non-maskable interrupt   | 0           | none                                              |
| SUCCESS                | response    | operation successful              | 0           | none                                              |
| SUCCESS_READ_PARAMETER | response    | operation successful              | 4           | parameter data (4 bytes)                          |
| INVALID_COMMAND        | response    | failure due to invalid command    | 0           | none                                              |
| INVALID_PARAMETER      | response    | failure due to invalid parameter  | 0           | none                                              |
| INVALID_DATA_LENGTH    | response    | failure due to invalid length     | 0           | none                                              |
| FAILURE                | response    | failure                           | 0           | none                                              |
| BUS_WRITE              | command     | write to bus                      | 1 - 256     | data                                              |
| SUCCESS_READ           | response    | operation successful              | 1 - 256     | data requested from a read request                |



| parameter_id           | description                       | value on read | value on write        |
| ---------------------- | --------------------------------- | ------------- | --------------------- |
| DEVICE_ID              | unique device id                  | device_id     | invalid               |   
| FIRMWARE_VERSION       | firmware version                  | fw_version    | invalid               |
| PROTOCOL_VERSION       | firmware version                  | fw_version    | invalid               |
| WRITE_ADDRESS          | address to use on next bus write  | next address  | next address          |
| CLOCK_SPEED            | generated clock speed             | TODO          | TODO                  |
| UART_SPEED             | communication uart speed          | TODO          | TODO                  |
| RESET_TIME             | TODO                              | TODO          | TODO                  |
| TIMEOUT                | TODO                              | TODO          | TODO                  |
| GPIO_TEST              | output level of test pin          | 0 or 1        | 0 for low, else high  |
