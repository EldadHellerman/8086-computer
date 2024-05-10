import isp
import serial
import serial.tools.list_ports
from time import sleep

import logging
logger = logging.getLogger(__name__)


class Companion:
    about = "8086 Companion Version 2.0"
    _serial: serial.Serial
    
    def __init__(serial: serial.Serial):
        """Opening and closing the serial port is not done by this class!
        It's the responsibility of the user."""
        _serial = serial
 
    def _packet_read(self) -> isp.Packet | None:
        input_buffer: bytearray = bytearray()
        # TODO add timeout?
        while(True):
            input = self._serial.read()
            if(input):
                input_buffer.extend(input)
                try:
                    logger.debug(f"read so far: {bytes(input).decode('ascii')}")
                except:
                    logger.debug(f"read so far: {input_buffer}")
            packet: isp.Packet = isp.Packet.parse_from_bytes(input_buffer)
            if(packet):
                return packet
            sleep(0.01)
        return None

    def _packet_write(self, packet_id: int, data: bytes):
        packet = list(isp.PREAMBLE) \
                + len(data).to_bytes(2, 'little') \
                + packet_id.to_bytes(1, 'little') \
                + list(data)
        logger.debug(f"writting packet: {packet}")
        self._serial.write(packet)

    def _packet_transaction(self, packet_id: int, data: bytes) -> isp.Packet | None:
        self._packet_write(packet_id, data)
        return self._packet_read()
    
    def parameter_read(self, parameter_id: int) -> int:
        data: bytes = parameter_id.to_bytes(1, 'little')
        r: isp.Packet = self._packet_transaction(isp.packet_id.READ_REG, data)
        if(r.packet_id != isp.packet_id.SUCCESS or len(r.data) != 4):
            return None
        return int.from_bytes(r.data, 'little')

    def parameter_write(self, parameter_id: int, value: int) -> bool:
        data: bytes = parameter_id.to_bytes(1, 'little') + value.to_bytes(4, 'little')
        r: isp.Packet = self._packet_transaction(isp.packet_id.PARAMETER_WRITE, data)
        if(r.packet_id != isp.packet_id.SUCCESS or len(r.data) != 0):
            return False
        return True

    





    def memory_read(self, address: int, length: int) -> list:
        # TODO support reading of more than 256 bytes using multiple requests.
        data: bytes = address.to_bytes(4, 'little') + length.to_bytes(1, 'little')
        return self._packet_transaction(isp.packet_id.BUS_READ, data)

    def memory_write(self, address: int, data: list) -> list:
        # TODO support writing of more than 256 bytes using multiple requests.
        return self._packet_transaction(isp.packet_id.BUS_WRITE, data)



    def flash_block_erase(self, block_address: int):
        # 8 blocks of 64kB
        lower_bank_block_addresses = [0x0_0000, 0x1_0000, 0x2_0000, 0x3_0000, 0x4_0000, 0x5_0000, 0x6_0000, 0x7_0000]
        # 7 blocks of 64kB, then 32kB, 8kB, 8kB, 16kB
        higher_bank_block_addresses = [0x0_0000, 0x1_0000, 0x2_0000, 0x3_0000, 0x4_0000, 0x5_0000, 0x6_0000,
                                0x7_0000, 0x7_800, 0x7_A000, 0x7_C0000]
        if block_address not in lower_bank_block_addresses:
            return
        
        self.memory_write(serial, 0x8_0AAA, [0xAA])
        self.memory_write(serial, 0x8_0555, [0x55])
        self.memory_write(serial, 0x8_0AAA, [0x80])
        self.memory_write(serial, 0x8_0AAA, [0xAA])
        self.memory_write(serial, 0x8_0555, [0x55])
        
        # chip erase:
        # parameter_write(serial, parameter_id.WRITE_ADDRESS, 0xAAA)
        # memory_write(serial, 1, [0x10])
        
        # block erase:
        address = 0x8_0000 | block_address
        self.memory_write(serial, address, [0x30])

    def flash_prog(self, address: int, data: int):
        self.memory_write(serial, 0x8_0AAA, [0xAA])
        self.memory_write(serial, 0x8_0555, [0x55])
        self.memory_write(serial, 0x8_0AAA, [0xA0])
        self.memory_write(serial, address, data.to_bytes(2, "little"))


    def write_file(self, file, address):
        pass

    



    def reset_coprocessor(self):
        print(f"resetting coprocessor")
        self._serial.dtr = True
        self._serial.dtr = False

    def read(self, start_address, data_length, output_file) -> bytes:
        """if output_file is none, returning data as bytes instead"""
        print(f"reading {data_length} bytes from {start_address}" + (f" to file {output_file}" if output_file else ""))

    
    def write(self, start_address, data_length, input_file):
        print(f"writing {data_length} bytes from file {input_file} to {start_address}")
    
    def write_to_flash(self, start_address, data_length, input_file):
        print(f"writing {data_length} bytes from file {input_file} to {start_address} (flash)")

    def reset_and_hold(self):
        print("preforming reset and hold")
    
    def hold(self):
        print("preforming hold")
    
    def release(self):
        print("preforming release")
    
    def generate_interrupt(self, interrupt_number: int):
        """interrupt_number = 256 is NMI"""
        print(f"generating interrupt number {interrupt_number}")

    def erase_flash(self, block_addresses: list[int], chip_erase: bool):
        if(chip_erase):
            print(f"flash chip erase")
        else:
            print(f"flash erasing blocks: {block_addresses}")
    
    def erase_flash(self, block_addresses: list[int], chip_erase: bool):
        if(chip_erase):
            print(f"flash chip erase")
        else:
            print(f"flash erasing blocks: {block_addresses}")
    
    def read_reg(self, register_number: int | None) -> bytes:
        # if register_number is none, read all register
        if(register_number is None):
            print(f"reading all registers")
        else:
            print(f"reading register #{register_number}")
        return (0).to_bytes(4, 'little')
    
    def write_reg(self, register_number: int, register_data: bytes):
        if(register_data is None or len(register_data) != 4):
            return
        print(f"writing {register_data} to register #{register_number}")
    
    def open_serial_port(self, com_port_number: int | None = None, baud_rate: int = 1000000, interactive_menu: bool = False):
        # open serial port from arguments, automatically, or with an interactive program
        msg_choose_port = "Available ports:\n"
        matching_port = None
        if(interactive_menu):
            ports = sorted(serial.tools.list_ports.comports())
            for i, (port, desc, hwid) in enumerate(ports):
                    # hwid is ["USB", VID:PID, SER, LOCATION]
                    vid_pid = hwid.split(' ')[1].removeprefix("VID:PID=")
                    if(matching_port is None and (isp.VID_PID is None or vid_pid == isp.VID_PID)):
                        matching_port = i
                    msg_choose_port += (f"    {i: 2}: {('*' if (matching_port == i) else '')}{desc}\n")
            msg_choose_port += "\nWhat port to use (enter for selected one)?"

            user_input = input(msg_choose_port)
            if(user_input != ''):
                i = int(user_input)
            com_port = ports[i][0]
        else:
            if(com_port_number is None):
                return False
            com_port = "COM" + str(com_port_number)
        self._serial = serial.Serial(None, baud_rate, timeout=0)
        self._serial.port = com_port
        self._serial.dtr = False
        self._serial.rts = False
        try:
            self._serial.open()
        except:
            pass
        if(not self._serial.is_open):
            return False
        return True
    
    def close_serial_port(self):
        self._serial.close()
