import random
import logging
from companion.companion import Companion
import isp
from time import sleep

logger = logging.getLogger(__name__)

companion = Companion()


def test_rw_memory(length, address):
    print(f"testing RW memory @0x{address:05X} ({length} bytes)")

    print("reading: ", companion.read(address, length))
    print("writing [1,2,3,...]: ", companion.write(length, [i for i in range(length)]))
    # print("memory write of 0x00000: ", companion.memory_write(length, [255 for i in range(length)]))
    print("reading: ", companion.read(address, length))
    
    print("writing [0,0,0,...]: ", companion.write(length, [0 for i in range(length)]))
    print("reading: ", companion.read(address, length))
    
    print("writing [255,255,255,...]: ", companion.write(length, [255 for i in range(length)]))
    print("reading: ", companion.read(address, length))
    
    random_data = [random.randint(0, 255) for i in range(length)]
    print("writing random data", companion.write(address, random_data))
    print("reading... data", ("Matched!" if (companion.read(address, length) == random_data) else "Mismatch."))

def test_flash():
    print(f"testing FLASH memory")
    address = 0x8_0000
    length = 64
    print("reading:")
    print(f"    0x{address:05X}: ", companion.memory_read(length, address)[2:])
    
    input("press enter to continue")
    print("erasing first block")
    companion.flash_block_erase(0x0_0000)
    sleep(0.5)
    print("reading:")
    print(f"    0x{address:05X}: ", companion.memory_read(length, address)[2:])
    print("programming flash:")
    companion.flash_prog(0x8_0000, 0x1234)
    companion.flash_prog(0x8_0002, 0x5678)
    companion.flash_prog(0x8_0010, 0x9ABC)
    print("reading:")
    print(f"    0x{address:05X}: ", companion.memory_read(length, address)[2:])

def flash_prog_initializing_program():
    # works only for lower memory bank!
    # else the address is different
    companion.flash_block_erase(0x7_0000)
    sleep(1)

    # disassembly:
    # bc 00 40             	mov    $0x4000,%sp
    # ea 00 04 00 00       	ljmp   $0x0,$0x400
    companion.flash_prog(0xffff0, 0x00BC)
    companion.flash_prog(0xffff2, 0xEA40)
    companion.flash_prog(0xffff4, 0x0400)
    companion.flash_prog(0xffff6, 0x0000)
    companion.flash_prog(0xffff8, 0x9090)
    companion.flash_prog(0xffffA, 0x9090)
    companion.flash_prog(0xffffC, 0x9090)
    companion.flash_prog(0xffffE, 0x9090)
    # infiinty loop:
    # flash_prog(serial, 0xffff0, 0xFEEB)

def program_file(file_path: str):
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
        companion.parameter_write(isp.parameter_id.WRITE_ADDRESS, address)
        chunk = data[data_written : data_written + 64]
        companion.memory_write(64, chunk)
        data_written += 64
        address += 64


def main():
    # open serial port without resetting board: (not asserting DTR and RTS)
    
    
    input("press enter to start")
    id = companion.read_reg(isp.Register_id.DEVICE_ID)
    print(f"device id is: 0x{id :08X}")
    print("hold:")
    companion.hold()
    # print(f"ADDRRES is: 0x{companion.parameter_read(isp.parameter_id.WRITE_ADDRESS) :08X}")
    # print("response of write address is:", companion.parameter_write(isp.parameter_id.WRITE_ADDRESS, 0x12345678))
    # print(f"ADDRRES is now: 0x{companion.parameter_read(isp.parameter_id.WRITE_ADDRESS) :08X}")


    # print("reset and hold:", companion.packet_transaction(isp.packet_id.RESET_AND_BUS_HOLD, []))
    # input("press enter to continue")
    # print("release:", companion.packet_transaction(isp.packet_id.BUS_RELEASE, []))
    # sleep(0.25)
    

    
    # for i in range(0, 256, 16):
    #     input("press enter to continue")
    #     print(f"memory read 0x{i:02X}: ", companion.memory_read(16, i))

    # input("press enter to continue")
    # test_rw_memory(64, 0x0_0000)
    # print()
    # test_rw_memory(64, 0x2_0000)
    # companion.write_to_memory(16, 0x1_0000)
    # input("press enter to continue")
    # companion.write_to_memory(16, 0x2_0000)

    # test_flash()
    # input("press enter to program initializing program to flash")
    # flash_prog_initializing_program()
    
    # print("reading 0x7F0: ", companion.memory_read(16, 0x07F0)[2:])

    input("press enter to load program to ram")
    program_file("./test assembly/prog2.bin")

    input("press enter reset and release for a few seconds")
    companion.reset_and_hold()
    companion.release()
    sleep(5)
    companion.hold()
    print("reading 0x000: ", companion.memory_read(16, 0x0000)[2:])
    print("reading 0x400: ", companion.memory_read(16, 0x0400)[2:])
    print("reading 0x600: ", companion.memory_read(16, 0x0600)[2:])
    print("reading 0x7F0: ", companion.memory_read(16, 0x07F0)[2:])
    print("reading 0x3FF0: ", companion.memory_read(16, 0x3FF0)[2:])
    print("reading 0xF_FFF0: ", companion.memory_read(16, 0xF_FFF0)[2:])

    companion.close_serial_port()
    print("bye!")

