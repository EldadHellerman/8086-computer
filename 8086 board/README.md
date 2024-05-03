# 8086 board
This folder is a KiCad project folder.
It includes the 8086 PC schematics and PCB design.

## Board details

### General
- The 8086 itself.
- 128/256kB SRAM + 1024kB flash with bank select switch (only 512kB are addressable at a time, so flash is divided into two banks, meaning two complete programs can be stored at once, and selected from using the switch before boot).
- Logic for address latching (including OR gate with bus ALE).
- Expansion port allowing access to 8086 bus.
This may be used for future developments - perhapse an FPGA graphics card with HDMI output.
- LED indicators for: latched adderss, memory access (SRAM 1, SRAM 2, Flash is A19, MCU, and LCD).
- Reset button.
- USB-C port for power and communication with coprocessor.


### IO
- 320*240 or 480*320 RGB 40 pin LCD.
- 32 Push Buttons.  
They form a keyboard, with 'a'-'z', space, enter, backspace, CTRL, ALT, SHIFT.
- Logic to latch data from buttons.


### Co-Processor
- Mircocontroller used to control the 8086 (generating clock and reset signals), programming the memory and generating interrupts.
- Clock source: if I dont want to generate 33% duty cycle, than maybe a lower frequency 50% duty cycle instead.
  (make 66% of 8MHz the 50% of frequency - which is 6MHz).
- Maybe also act as peripherals (with or without interrupts): timers, UART, mouse (input from touch screen), SD card interface, etc...
- USB to UART convertor (CH340C).
- USB-C port for power and to connect to computer.
- UART connector (4p header pins or JST-XH).


## Address Space

### Segments and physical address

The 8086 uses 20-bit addressing (for 1MB total).  
The physical address is calculated from a 16-bit segment register and a 16-bit address register:

```
physical address = (segment << 4) + offset
```

For convenience, I will think of each segment as being only the 4 highest order bits (0x?000), and the memory as being comprised of 16 segments, each 64kB in size.
    
For in / out instructions, only 16-bits addressing is allowed (first segment only).

The 8086 boots to address 0xFFFF0, so there must be some memory there. That's why the flash is at the top of the memory map.  
Interrupt vector table is at 0x000 - 0x100. That's why the SRAM is at the bottom of the memory map.

<details>
<summary>Tables of memory and IO maps:</summary>

### Memory map

| Addresses             | Segments  | Size  | Description                       |
| --------------------- | --------- | ----- | --------------------------------- |
| 0x0_0000 - 0x2_0000   | 0 - 1     | 128kB | SRAM                              |
| 0x2_0000 - 0x4_0000   | 2 - 3     | 128kB | optional additional SRAM          |
| 0x4_0000 - 0x6_0000   | 4 - 5     | 128kB | MMIO - Microcontroller            |
| 0x6_0000 - 0x7_0000   | 6         | 64 kB | MMIO - RGB LCD - DATA             |
| 0x7_8000 - 0x8_0000   | 7         | 64 kB | MMIO - RGB LCD  - COMMAND         |
| 0x8_0000 - 0xF_FFFF   | 8 - 15    | 512kB | Flash                             |

### IO map

IO ports are 16-bit wide.

| Number    | Address         | Read description | Write description    |
| --------- | --------------- | ---------------- | -------------------- |
| 0         | 0x0000          | 16 Push buttons. | FREE                 |
| 1         | 0x0002          | 16 Push buttons. | FREE                 |
| 2 - 7     | 0x0004 - 0x000E | FREE             | FREE                 |

</details>