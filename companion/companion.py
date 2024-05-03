from time import sleep
from isp import *
import serial
import random

port = "COM7"
baud_rate = 1000000
debug_enabeld = False



color_foreground = {'BLACK': '\x1B[30m', 'RED': '\x1B[31m', 'GREEN': '\x1B[32m', 'YELLOW': '\x1B[33m', 'BLUE': '\x1B[34m', 'MAGENTA': '\x1B[35m', 'CYAN': '\x1B[36m', 'WHITE': '\x1B[37m', 'RESET': '\x1B[39m'}
RESET_ALL = '\x1B[0m'

def print_debug(s: str, color: str = "RED"):
    if(not debug_enabeld):
        return
    print(color_foreground[color], end = '    DEBUG: ')
    print(s, end = ' ')
    print(RESET_ALL)

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
        # try:
        #     print_debug(f"read so far: '{bytes(input).decode('ascii')}'", "BLUE")
        # except:
        #     print_debug(f"read so far: {input}", "BLUE")
        # sleep(0.5)
        sleep(0.01)
    return input

def packet_write(serial: serial.Serial, packet_id: int, data: list):
    # print(f"    - packet write:",) #######################################################
    # if(_0_is_256 and length == 256): length = 0
    length = len(data)
    if(length == 256): length = 0
    print_debug(f"packet write: {[packet_id, length] + data}")
    serial.write([packet_id, length] + data)

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
    if(length == 256): length = 0
    return packet_transaction(serial, packet_id.BUS_READ, list(address.to_bytes(4, "little")) + list(length.to_bytes(1, "little")))

def memory_write(serial: serial.Serial, length: int, data: list) -> list:
    # TODO support writing of more than 256 bytes using multiple requests.
    if not (0 < length <= 256): return None
    return packet_transaction(serial, packet_id.BUS_WRITE, data)

def test_rw_memory(serial, length, address):
    print(f"testing RW memory @0x{address:05X} ({length} bytes)")
    parameter_write(serial, parameter_id.WRITE_ADDRESS, address)
    
    print("reading: ", memory_read(serial, length, address))
    print("writing [1,2,3,...]: ", memory_write(serial, length, [i for i in range(length)]))
    # print("memory write of 0x00000: ", memory_write(serial, length, [255 for i in range(length)]))
    print("reading: ", memory_read(serial, length, address))
    
    print("writing [0,0,0,...]: ", memory_write(serial, length, [0 for i in range(length)]))
    print("reading: ", memory_read(serial, length, address))
    
    print("writing [255,255,255,...]: ", memory_write(serial, length, [255 for i in range(length)]))
    print("reading: ", memory_read(serial, length, address))
    
    random_data = [random.randint(0, 255) for i in range(length)]
    print("writing random data", memory_write(serial, length, random_data))
    print("reading... data", ("Matched!" if (memory_read(serial, length, address)[2:] == random_data) else "Mismatch."))

def flash_block_erase(serial: serial.Serial, block_address: int):
    # 8 blocks of 64kB
    lower_bank_block_addresses = [0x0_0000, 0x1_0000, 0x2_0000, 0x3_0000, 0x4_0000, 0x5_0000, 0x6_0000, 0x7_0000]
    # 7 blocks of 64kB, then 32kB, 8kB, 8kB, 16kB
    higher_bank_block_addresses = [0x0_0000, 0x1_0000, 0x2_0000, 0x3_0000, 0x4_0000, 0x5_0000, 0x6_0000,
                            0x7_0000, 0x7_800, 0x7_A000, 0x7_C0000]
    if block_address not in lower_bank_block_addresses:
        return
    
    parameter_write(serial, parameter_id.WRITE_ADDRESS, 0x8_0AAA)
    memory_write(serial, 1, [0xAA])
    parameter_write(serial, parameter_id.WRITE_ADDRESS, 0x8_0555)
    memory_write(serial, 1, [0x55])
    parameter_write(serial, parameter_id.WRITE_ADDRESS, 0x8_0AAA)
    memory_write(serial, 1, [0x80])
    parameter_write(serial, parameter_id.WRITE_ADDRESS, 0x8_0AAA)
    memory_write(serial, 1, [0xAA])
    parameter_write(serial, parameter_id.WRITE_ADDRESS, 0x8_0555)
    memory_write(serial, 1, [0x55])
    
    # chip erase:
    # parameter_write(serial, parameter_id.WRITE_ADDRESS, 0xAAA)
    # memory_write(serial, 1, [0x10])
    
    # block erase:
    address = 0x8_0000 | block_address
    parameter_write(serial, parameter_id.WRITE_ADDRESS, address)
    memory_write(serial, 1, [0x30])


def flash_prog(serial: serial.Serial, address: int, data: int):
    parameter_write(serial, parameter_id.WRITE_ADDRESS, 0x8_0AAA)
    memory_write(serial, 1, [0xAA])
    parameter_write(serial, parameter_id.WRITE_ADDRESS, 0x8_0555)
    memory_write(serial, 1, [0x55])
    parameter_write(serial, parameter_id.WRITE_ADDRESS, 0x8_0AAA)
    memory_write(serial, 1, [0xA0])
    parameter_write(serial, parameter_id.WRITE_ADDRESS, address)
    memory_write(serial, 1, list(data.to_bytes(2, "little")))

def test_flash(serial: serial.Serial):
    print(f"testing FLASH memory")
    address = 0x8_0000
    length = 64
    print("reading:")
    print(f"    0x{address:05X}: ", memory_read(serial, length, address)[2:])
    
    input("press enter to continue")
    print("erasing first block")
    flash_block_erase(serial, 0x0_0000)
    sleep(0.5)
    print("reading:")
    print(f"    0x{address:05X}: ", memory_read(serial, length, address)[2:])
    print("programming flash:")
    flash_prog(serial, 0x8_0000, 0x1234)
    flash_prog(serial, 0x8_0002, 0x5678)
    flash_prog(serial, 0x8_0010, 0x9ABC)
    print("reading:")
    print(f"    0x{address:05X}: ", memory_read(serial, length, address)[2:])

def flash_prog_initializing_program(serial: serial.Serial):
    # works only for lower memory bank!
    # else the address is different
    flash_block_erase(serial, 0x7_0000)
    sleep(1)

    # disassembly:
    # bc 00 40             	mov    $0x4000,%sp
    # ea 00 04 00 00       	ljmp   $0x0,$0x400
    flash_prog(serial, 0xffff0, 0x00BC)
    flash_prog(serial, 0xffff2, 0xEA40)
    flash_prog(serial, 0xffff4, 0x0400)
    flash_prog(serial, 0xffff6, 0x0000)
    flash_prog(serial, 0xffff8, 0x9090)
    flash_prog(serial, 0xffffA, 0x9090)
    flash_prog(serial, 0xffffC, 0x9090)
    flash_prog(serial, 0xffffE, 0x9090)
    # infiinty loop:
    # flash_prog(serial, 0xffff0, 0xFEEB)

def program_file(serial: serial.Serial, file_path: str):
    f = open(file_path, 'rb')
    data = list(f.read())
    if(len(data) % 64 != 0):
        data.extend([0] * (64 - (len(data) % 64)))
    # for i in range(0, len(data), 2):
    #     temp = data[i]
    #     data[i] = data[i + 1]
    #     data[i + 1] = temp
    address = 0
    data_written = 0
    while(data_written != len(data)):
        parameter_write(serial, parameter_id.WRITE_ADDRESS, address)
        chunk = data[data_written : data_written + 64]
        memory_write(serial, 64, chunk)
        data_written += 64
        address += 64
