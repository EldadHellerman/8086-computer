ISP_DATA_RANGE_1_TO_256_FLAG = 1 << 7
PACKET_HEADER_LENGTH = 2

class packet_id:
    PARAMETER_READ = 0
    PARAMETER_WRITE = 1
    RESET_AND_BUS_HOLD = 2
    BUS_HOLD = 3
    BUS_RELEASE = 4
    BUS_READ = 5
    GENERATE_INTERRUPT = 6
    GENERATE_NMI = 7
    SUCCESS = 8
    SUCCESS_READ_PARAMETER = 9
    INVALID_COMMAND = 10
    INVALID_PARAMETER = 11
    INVALID_DATA_LENGTH = 12
    FAILURE = 13

    BUS_WRITE = 128
    SUCCESS_READ = 129

class parameter_id:
    DEVICE_ID = 0
    FIRMWARE_VERSION = 1
    PROTOCOL_VERSION = 2
    WRITE_ADDRESS = 3
    CLOCK_SPEED = 4
    UART_SPEED = 5
    RESET_TIME = 6
    TIMEOUT = 7
    GPIO_TEST = 8
