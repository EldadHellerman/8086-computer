#ifndef ISP_H_
#define ISP_H_

#define ISP_PROTOCOL_VERSION        0x1000

// packet header are bytes that come before the data itself.
#define ISP_PACKET_HEADER_LENGTH        2
// packet footer are bytes that come after the data, like checksum and a suffix.
#define ISP_PACKET_FOOTER_LENGTH        0
// maximum packet data length
#define ISP_PACKET_MAX_DATA_LENGTH      256
// maximum total packet length, which is the sum of the lengths of the header, data, and footer
#define ISP_PACKET_MAX_PACKET_LENGTH    (     ISP_PACKET_HEADER_LENGTH \
                                            + ISP_PACKET_MAX_DATA_LENGTH \
                                            + ISP_PACKET_FOOTER_LENGTH )

// index in packet of packet_id field
#define ISP_FIELD_INDEX_PACKET_ID        0
// index in packet of data_length field
#define ISP_FIELD_INDEX_DATA_LENGTH      1
// index in packet of data field
#define ISP_FIELD_INDEX_DATA_START       2

#define ISP_DATA_LENGTH_PARAMETER_READ          1
#define ISP_DATA_LENGTH_PARAMETER_WRITE         5
#define ISP_DATA_LENGTH_RESET_AND_HOLD_BUS      0
#define ISP_DATA_LENGTH_BUS_HOLD                0
#define ISP_DATA_LENGTH_BUS_RELEASE             0
#define ISP_DATA_LENGTH_BUS_READ                5
#define ISP_DATA_LENGTH_GENERATE_INTERRPUT      1
#define ISP_DATA_LENGTH_GENERATE_NMI            0

#define ISP_DATA_LENGTH_SUCCESS                 0
#define ISP_DATA_LENGTH_SUCCESS_READ_PARAMETER  4
#define ISP_DATA_LENGTH_INVALID_COMMAND         0
#define ISP_DATA_LENGTH_INVALID_PARAMETER       0
#define ISP_DATA_LENGTH_INVALID_DATA_LENGTH     0
#define ISP_DATA_LENGTH_INVALID_FAILURE         0

#define ISP_DATA_RANGE_1_TO_256_FLAG        (1 << 7)

typedef enum {
    // packets where data length is 0-255:
    PARAMETER_READ = 0,
    PARAMETER_WRITE,
    RESET_AND_BUS_HOLD,
    BUS_HOLD,
    BUS_RELEASE,
    BUS_READ,
    GENERATE_INTERRUPT,
    GENERATE_NMI,
    SUCCESS,
    SUCCESS_READ_PARAMETER,
    INVALID_COMMAND,
    INVALID_PARAMETER,
    INVALID_DATA_LENGTH,
    FAILURE,
    

    // packets where data length is 1-256:
    // so when reading packet, 1 should be added to data length
    BUS_WRITE = ISP_DATA_RANGE_1_TO_256_FLAG,
    SUCCESS_READ
} isp_packet_id;

typedef enum {
    PARAMETER_DEVICE_ID,
    PARAMETER_FIRMWARE_VERSION,
    PARAMETER_PROTOCOL_VERSION,
    PARAMETER_WRITE_ADDRESS,
    PARAMETER_GPIO_TEST,
    PARAMETER_CLOCK_SPEED,
    PARAMETER_UART_SPEED,
    PARAMETER_RESET_TIME,
    PARAMETER_TIMEOUT
} isp_parameter_id;

#endif /* ISP_H_ */