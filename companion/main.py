import logging
import argparse
import shlex
import sys
from companion import Companion

color_foreground = {'BLACK': '\x1B[30m', 'RED': '\x1B[31m', 'GREEN': '\x1B[32m', 'YELLOW': '\x1B[33m', 'BLUE': '\x1B[34m', 'MAGENTA': '\x1B[35m', 'CYAN': '\x1B[36m', 'WHITE': '\x1B[37m', 'RESET': '\x1B[39m'}

logger = logging.getLogger(__name__)
logging.basicConfig(filename='log.log', encoding='utf-8', level=logging.DEBUG)

companion = Companion()

# {command name: its description}, used as valid commands choices, as well as to generate an informative description.
commands: dict[str, str] = {
    "reset-coprocessor":    "reset coprocessor board",
    "read":                 "read from bus",
    "write":                "write to bus",
    "program":              "write to flash (programms flash)",
    "reset":                "resets 8086 and holds bus",
    "hold":                 "hold bus (stops 8086 code execution)",
    "release":              "release bus",
    "interrupt":            "generate interrupt",
    "reg_read":             "read a register",
    "reg_write":            "write to a register",
    "erase":                "erase flash",
    "help":                 "print help menu"
}

commands_interactive = {
    "quit":         "quit interactive session"
}

help_from_commands = lambda commands: "Available commands are:\n" + '\n'.join([f"\t {name:20} \t {desc}" for (name, desc) in commands.items()])



class ErrorCatchingArgumentParser(argparse.ArgumentParser):
    def exit(self, status=0, message=None):
        if status:
            raise argparse.ArgumentError(None, f'error: {message}')

parser: ErrorCatchingArgumentParser = None

def get_parser(interactive_or_cmd: bool) -> ErrorCatchingArgumentParser:
    description = ""
    epilog = "\n\n" + companion.about + "\nEldad Hellerman"
    if(interactive_or_cmd):
        description += help_from_commands(commands | commands_interactive)
        parser = ErrorCatchingArgumentParser(exit_on_error=False, formatter_class=argparse.RawDescriptionHelpFormatter, description=description, add_help=False, epilog=epilog)
    else:
        description += "To start an interactive Session, call the program without any arguments.\n\n" + help_from_commands(commands)
        parser = argparse.ArgumentParser(formatter_class=argparse.RawDescriptionHelpFormatter, description=description, add_help=False, epilog=epilog)
    
    type_autoint = lambda x: int(x,0)
    type_interrupt = lambda x: 256 if(x.upper()=="NMI") else int(x,0)
    type_uint32_t = lambda x: int(x,0).to_bytes(4, 'little')

    parser.add_argument("-s", "--start-address", metavar='start address', type=type_autoint, help="start address of read", dest="start-address")
    parser.add_argument("-e", "--end-address", metavar='end adresss', type=type_autoint, help="end address of read", dest="end-address")
    parser.add_argument("-l", "--length", metavar='length', type=type_autoint, help="length of data", dest="data-length")

    parser.add_argument("-o", metavar='output file', help="dump memory to file", dest="output-file")
    parser.add_argument("-i", metavar='input file', help="get input from file", dest="input-file")

    choices = list(commands) + (list(commands_interactive) if interactive_or_cmd else [])
    parser.add_argument("command", metavar='command', choices= choices, help="command to execute")

    parser.add_argument("-n", metavar='interrupt number', type=type_interrupt, help="interrupt number | 'nmi' (256 == 'nmi')", dest="interrupt-number")
    parser.add_argument("-r", metavar='register number', type=type_autoint, help="register name or number to use for read / write", dest="register-number")
    parser.add_argument("-d", metavar='register data', type=type_uint32_t, help="data to use for register write", dest="register-data")
    blocks_chip_erase = parser.add_mutually_exclusive_group()
    blocks_chip_erase.add_argument("-a", action="store_true", help="full chip erase", dest="chip-erase")
    blocks_chip_erase.add_argument("-b", metavar='flash block addresses', help="addresses of flash blocks to erase", nargs="+", dest="blocks")
    
    if(not interactive_or_cmd):
        parser.add_argument("-c", metavar='com port', help="Serial com port number", type=int, dest="com-port")
        parser.add_argument("--baud-rate", metavar='baud rate', help="Serial buad rate", type=int, dest="baud-rate", default=1000000)
    return parser

def get_start_address_and_length(start_address, end_address, data_length) -> tuple[int]:
    # exactly two of them must be specified:
    if(bool(start_address) + bool(end_address) + bool(data_length) != 2):
        return None
    if(not start_address):
        return (end_address - data_length, data_length)
    if(not end_address):
        return (start_address, data_length)
    if(not data_length):
        return (start_address, end_address - start_address)

def execute_read(start_address, end_address, data_length, output_file):
    start_address_and_data_length = get_start_address_and_length(start_address, end_address, data_length)
    if(not start_address_and_data_length):
        print("exactly two of the following must be specified: (start address, end address, data_length)")
        return
    start_address = start_address_and_data_length[0]
    data_length = start_address_and_data_length[1]

    if(output_file):
        try:
            with open(output_file, 'wb') as file:
                companion.read(start_address, data_length, file)
        except FileNotFoundError as e:
            print("output file is invalid.")
            print(e)
    else:
        companion.read(start_address, data_length, output_file=None)

def execute_write(start_address, end_address, data_length, input_file):
    valid = True
    start_address_and_data_length = get_start_address_and_length(start_address, end_address, data_length)
    if(not start_address_and_data_length):
        print("exactly two of the following must be specified: (start address, end address, data_length)")
        valid = False
    if(not input_file):
        print("input file must be specified")
        valid = False
    if(not valid):
        return
    start_address = start_address_and_data_length[0]
    data_length = start_address_and_data_length[1]
    
    try:
        with open(input_file, 'rb') as file:
            companion.write(start_address, data_length, file)
    except FileNotFoundError as e:
        print("input file is invalid.")
        print(e)

def execute_write_to_flash(start_address, end_address, data_length, input_file):
    valid = True
    start_address_and_data_length = get_start_address_and_length(start_address, end_address, data_length)
    if(not start_address_and_data_length):
        print("exactly two of the following must be specified: (start address, end address, data_length)")
        valid = False
    if(not input_file):
        print("input file must be specified")
        valid = False
    if(not valid):
        return
    start_address = start_address_and_data_length[0]
    data_length = start_address_and_data_length[1]
    
    try:
        with open(input_file, 'rb') as file:
            companion.write_to_flash(start_address, data_length, file)
    except FileNotFoundError as e:
        print("input file is invalid.")
        print(e)
    
def execute_generate_interrupt(interrupt_number: str):
    if(not interrupt_number):
        print("interrupt number must be specified (or 'nmi')")
        return
    if(not 0 <= interrupt_number <= 256):
        print("interrupt number is out of range")
        return
    
    companion.generate_interrupt(interrupt_number)

def execute_reg_write(register_number, register_data):
    valid = True
    if(register_number is None):
        print("register number must be specified")
        valid = False
    if(register_data is None):
        print("register data must be specified")
        valid = False
    if(not valid):
        return
    
    companion.write_reg(register_number, register_data)

def execute(arguments):
    # print(f"executing arguments: {arguments}")
    command = arguments["command"]
    start_address = arguments["start-address"]
    end_address = arguments["end-address"]
    data_length = arguments["data-length"]
    output_file = arguments["output-file"]
    input_file = arguments["input-file"]
    interrupt_number = arguments["interrupt-number"]
    register_number = arguments["register-number"]
    register_data = arguments["register-data"]
    chip_erase = arguments["chip-erase"]
    blocks = arguments["blocks"]

    match command:
        case "reset-coprocessor":
            companion.reset_coprocessor()
        case "read":
            execute_read(start_address, end_address, data_length, output_file)
        case "write":
            execute_write(start_address, end_address, data_length, input_file)
        case "program":
            execute_write_to_flash(start_address, end_address, data_length, input_file)
        case "reset":
            companion.reset_and_hold()
        case "hold":
            companion.hold()
        case "release":
            companion.release()
        case "interrupt":
            execute_generate_interrupt(interrupt_number=interrupt_number)
        case "reg_read":
            # TODO change to pretty print
            print(companion.read_reg(register_number))
        case "reg_write":
            execute_reg_write(register_number, register_data)
        case "erase":
            companion.erase_flash(blocks, chip_erase)
        case 'help':
            parser.print_help()
    return
        

def main():
    global parser
    interactive_mode = (len(sys.argv) == 1)
    parser = get_parser(interactive_mode)
    if(not interactive_mode):
        arguments = vars(parser.parse_args())
        if(not companion.open_serial_port(arguments["com-port"], arguments["baud-rate"])):
            print("Could not open serial port.")
            return
        try:
            execute(arguments)
        except:
            companion.close_serial_port()
            raise
        return
    
    if(not companion.open_serial_port(interactive_menu=True)):
        print("Could not open serial port.")
        return

    user_input = ""
    while(True):
        try:
            user_input += input(">" if(len(user_input) == 0) else "\t")
            if(len(user_input) > 0 and user_input[-1] == '\\'):
                user_input = user_input[:-1] + " "
                continue
            argv = shlex.split(user_input)
            if(len(argv) == 0):
                continue
            arguments = vars(parser.parse_args(argv))
        except argparse.ArgumentError as e:
            user_input = ""
            print(color_foreground['YELLOW'], "Error:", e, color_foreground['RESET'])
        else:
            user_input = ""
            if(arguments['command'] == "quit"):
                break
            execute(arguments)
    
    companion.close_serial_port()

if __name__ == "__main__":
    main()
    