from time import sleep
import serial

port = "COM7"
baud_rate = 1000000


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

def is_packet_complete(packet):
    packet_length = len(packet)
    if(packet_length < PACKET_HEADER_LENGTH):
        return False
    packet_id = packet[0]
    packet_data_length = packet[1]

    # check if packet is with 1 to 256 data length range
    if((packet_id & ISP_DATA_RANGE_1_TO_256_FLAG) and  (packet_data_length == 0)):
        packet_data_length = 256
    # print(f"checking if packet length of {packet_length} is equal to {PACKET_HEADER_LENGTH + packet_data_length}")
    return (packet_length == PACKET_HEADER_LENGTH + packet_data_length)

def packet_read(serial) -> list:
    input = []
    while(True):
        d = bytes(serial.read(100))
        input.extend(d)
        if(is_packet_complete(input)):
            break
        try:
            print(f"read {bytes(input).decode('ascii')} so far")
        except:
            print(f"read {input} so far")
        sleep(0.5)
        # sleep(0.05)
    return input

def packet_write(serial: serial.Serial, packet_id: int, data: list):
    print(f"    - packet write:",[packet_id, len(data)] + data) #######################################################
    # if(_0_is_256 and length == 256): length = 0
    serial.write([packet_id, len(data)] + data)

def packet_transaction(serial: serial.Serial, packet_id: int, data: list) -> list:
    packet_write(serial, packet_id, data)
    return packet_read(serial)

def parameter_read(serial: serial.Serial, parameter_id: int) -> list:
    r = packet_transaction(serial, packet_id.PARAMETER_READ, [parameter_id])
    if(r[0] == packet_id.SUCCESS_READ_PARAMETER and r[1] == 4):
        return int.from_bytes(r[2:6], "little")

def parameter_write(serial: serial.Serial, parameter_id: int, value: list) -> list:
    return packet_transaction(serial, packet_id.PARAMETER_WRITE, [parameter_id] + list(value.to_bytes(4, "little")))

def memory_read(serial: serial.Serial, length: int, address: int) -> list:
    # TODO support reading of more than 256 bytes using multiple requests.
    return packet_transaction(serial, packet_id.BUS_READ, list(address.to_bytes(4, "little")) + list(length.to_bytes(1, "little")))

def memory_write(serial: serial.Serial, length: int, data: list) -> list:
    # TODO support writing of more than 256 bytes using multiple requests.
    if not (0 < length <= 256): return None
    return packet_transaction(serial, packet_id.BUS_WRITE, data,)

def main():
    s = serial.Serial(port, baud_rate, timeout=0)
    if(not s.is_open):
        print("Could not open serial port.")
        return
    print("Serial port open!")
    sleep(2) # sleepig for a bit to let arduino reset itself
    
    print(f"device id is: 0x{parameter_read(s, parameter_id.DEVICE_ID) :08X}")
    # print(f"ADDRRES is: 0x{parameter_read(s, parameter_id.WRITE_ADDRESS) :08X}")
    # print("response of write address is:", parameter_write(s, parameter_id.WRITE_ADDRESS, 0x12345678))
    # print(f"ADDRRES is now: 0x{parameter_read(s, parameter_id.WRITE_ADDRESS) :08X}")


    print("reset and hold:", packet_transaction(s, packet_id.RESET_AND_BUS_HOLD, []))
    print("release:", packet_transaction(s, packet_id.BUS_RELEASE, []))
    sleep(1)
    print("hold:", packet_transaction(s, packet_id.BUS_HOLD, []))

    while(1):
        print("memory read: ", memory_read(s, 16, 0x20000))
    parameter_write(s, parameter_id.WRITE_ADDRESS, 0x20000)
    print("memory write of 0x00000: ", memory_write(s, 16, [i for i in range(16)]))
    print("memory read: ", memory_read(s, 16, 0x20000))

    print("bye!")
    s.close()

if __name__ == "__main__":
    main()