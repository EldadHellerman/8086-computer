from companion import *

def main():
    # open serial port without resetting board: (not asserting DTR and RTS)
    s = serial.Serial(None, baud_rate, timeout=0)
    s.port = port
    s.dtr = False
    s.rts = False
    s.open()
    if(not s.is_open):
        print("Could not open serial port.")
        return
    print("Serial port open!")
    
    input("press enter to start")
    print(f"device id is: 0x{parameter_read(s, parameter_id.DEVICE_ID) :08X}")
    print("hold:", packet_transaction(s, packet_id.BUS_HOLD, []))
    # print(f"ADDRRES is: 0x{parameter_read(s, parameter_id.WRITE_ADDRESS) :08X}")
    # print("response of write address is:", parameter_write(s, parameter_id.WRITE_ADDRESS, 0x12345678))
    # print(f"ADDRRES is now: 0x{parameter_read(s, parameter_id.WRITE_ADDRESS) :08X}")


    # print("reset and hold:", packet_transaction(s, packet_id.RESET_AND_BUS_HOLD, []))
    # input("press enter to continue")
    # print("release:", packet_transaction(s, packet_id.BUS_RELEASE, []))
    # sleep(0.25)
    

    
    # for i in range(0, 256, 16):
    #     input("press enter to continue")
    #     print(f"memory read 0x{i:02X}: ", memory_read(s, 16, i))

    # input("press enter to continue")
    # test_rw_memory(s, 64, 0x0_0000)
    # print()
    # test_rw_memory(s, 64, 0x2_0000)
    # write_to_memory(s, 16, 0x1_0000)
    # input("press enter to continue")
    # write_to_memory(s, 16, 0x2_0000)

    # test_flash(s)
    # input("press enter to program initializing program to flash")
    # flash_prog_initializing_program(s)
    
    # print("reading 0x7F0: ", memory_read(s, 16, 0x07F0)[2:])

    input("press enter to load program to ram")
    program_file(s, "./test assembly/prog2.bin")

    input("press enter reset and release for a few seconds")
    print("reset and hold:", packet_transaction(s, packet_id.RESET_AND_BUS_HOLD, []))
    print("release:", packet_transaction(s, packet_id.BUS_RELEASE, []))
    sleep(5)
    print("hold:", packet_transaction(s, packet_id.BUS_HOLD, []))
    print("reading 0x000: ", memory_read(s, 16, 0x0000)[2:])
    print("reading 0x400: ", memory_read(s, 16, 0x0400)[2:])
    print("reading 0x600: ", memory_read(s, 16, 0x0600)[2:])
    print("reading 0x7F0: ", memory_read(s, 16, 0x07F0)[2:])
    print("reading 0x3FF0: ", memory_read(s, 16, 0x3FF0)[2:])
    print("reading 0xF_FFF0: ", memory_read(s, 16, 0xF_FFF0)[2:])

    print("bye!")
    s.close()

if __name__ == "__main__":
    main()