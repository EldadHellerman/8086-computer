# See "ISP protocol.md" for more information.

from typing import Self

VID_PID = "1A86:7523"
VERSION: bytes = 0x0201.to_bytes(4, 'little')
PREAMBLE: bytes = 0xAA55.to_bytes(2, 'little')

class Packet_id:
    READ_REG    = 0x00
    WRITE_REG   = 0x01
    READ_BUS    = 0x02
    WRITE_BUS   = 0x03
    WRITE_FLASH = 0x04
    SUCCESS     = 0x10
    FAILUR      = 0x11

class Register_id:
    STATUS              = 0
    DEVICE_ID           = 1
    FIRMWARE_VERSION    = 2
    PROTOCOL_VERSION    = 3
    CLOCK_SPEED         = 4
    UART_SPEED          = 5
    RESET_TIME          = 6
    TIMEOUT             = 7
    GPIO_TEST           = 8
    RESET               = 9
    BUS_HOLD            = 10
    INTERRUPT           = 11

class Packet_length:
    READ_REG    = 1
    WRITE_REG   = 5
    READ_BUS    = 6

class Packet:
    packet_id : bytes
    data : bytes
    
    def __init__(self, packet_id: bytes, data: bytes):
        self.packet_id = packet_id
        self.data = data
    
    def is_valid(self) -> bool:
        return((self.preamble == PREAMBLE) and (self.data_length == len(self.data)))

    @classmethod
    def parse_from_bytes(input: bytes | bytearray) -> Self | None:
        preamble = input[0:2]
        packet_id = input[2]
        data_length = input[3:5]
        data = input[5:]
        if(preamble != PREAMBLE) or (data_length == len(data)):
            return None
        return Packet(packet_id, data)
    

