# ISP protocol
Version 2.0  

The ISP protocol is used for communicating with the coprocessor.
This communication happens over serial, using the coprocessor UART to USB adapter.  

General structure:
- The protocol consists of packets.
- The protocol is little endian (i.e 0x1234 is sent as 0x34 and then 0x12). This matches with UART being LSB first.
- Each packet is either a command or a response. Each command is answered with a response.
- Only the computer may send commands, Only the coprocessor may send responses.
- Registers are used to control different functions, and set different settings of the coprocessor.
- Registers are identified by a 1-byte number, and are 4-bytes wide (although using only parts of the bits is possible).


Packets have the following fields (at that order):
| field name    | size (bytes)  |
| ------------- | ------------- |
| preamble      | 2             |
| packet id     | 1             |
| data length   | 2             |
| data          | 0 - 65,535    |

For preamble, the value 0xAA55 is used.


## Packets:
| packet id | name        | type     | description                   | data length | data (length in bytes)         |
| --------- | ----------- | -------- | ----------------------------- | ----------- | ------------------------------ |
| 0x00      | READ_REG    | command  | read from a register          | 1           | register number (1)            |
| 0x01      | WRITE_REG   | command  | write to a register           | 5           | register number (1), value (4) |
| 0x02      | READ_BUS    | command  | read from the bus             | 6           | address (4), length (2)        |
| 0x03      | WRITE_BUS   | command  | write to the bus              | variable    | address (4), data (variable)   |
| 0x04      | SUCCESS     | response | successful operation          | 0           | none                           |
|           |             |          | successful READ_REGISTER      | 4           | value of register (4)          |
|           |             |          | successful READ_BUS           | variable    | data (variable)                |
| 0x05      | FAILURE     | response | failure                       | 0           | none                           |



## Registers:
| number    | name             | value on read | value on read description     | value on write | value on write description         |
| --------- | ---------------- | ------------- | ----------------------------- | -------------- | ---------------------------------- |
| 0x00      | STATUS           | status        | status. indicates error types TODO | status    |                                    |
| 0x01      | DEVICE_ID        | 0xXXXXXXXX    | unique device id              |                |                                    |
| 0x02      | FIRMWARE_VERSION | 0xXXXXXXXX    | firmware version              |                |                                    |
| 0x03      | PROTOCOL_VERSION | 0xXXXXXXXX    | protocol version              |                |                                    |
| 0x04      | CLOCK_SPEED      | TODO          | generated clock speed         | TODO           | same                               |
| 0x05      | UART_SPEED       | TODO          | communication uart speed      | TODO           | same                               |
| 0x06      | RESET_TIME       | TODO          | TODO                          | TODO           | same                               |
| 0x07      | TIMEOUT          | TODO          | TODO                          | TODO           | same                               |
| 0x08      | GPIO_TEST        | 0 - 1         | input level of test pin       | 0 - 1          | output level of test pin           |
| 0x10      | RESET            |               |                               | 1              | Resets the 8086                    |
|           |                  |               |                               | 2              | Resets 8086 and holds bus          |
| 0x11      | BUS_HOLD         | 0 - 1         | Bus on hold indicator         | 0 - 1          | Release or hold bus                |
| 0x12      | INTERRUPTS       |               |                               | 0 - 255        | Generates corresponding interrupts |
|           |                  |               |                               | 256            | Generates NMI                      |