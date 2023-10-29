# 8086 board
This folder is a KiCad project folder.
It includes the 8086 PC schematics and PCB design, as well as this readme file.

<details>
<summary>Tables of IC's I have lying around:</summary>

## Logic

| IC Name           | Description                               | amount    |
| ----------------- | ----------------------------------------- | --------- |
| 74AC373           | Octal latch with 3-STATE Outputs          | 20+       |
| 74HC138D          | 3 to 8 line decoder; inverting            | 6         |
| 74AC00 / 74HC132  | Quad 2-input NAND gate / schmitt trigger  | 2 / 6     |
| 74HC02            | Quad 2-input NOR gate                     | 6         |
| 74ACT08           | Quad 2-input AND gate                     | 2         |
| 74HC14 / HEF40106 | Hex inverting Schmitt trigger             | 1 / 1     |

## Memory

| IC Name       | Description         | amount    |
| ------------- | ------------------- | --------- |
| IS61C256AH    | SRAM 32K x 8        | 5         |
| CXK58257AM    | SRAM 32K x 8        | 1         |
| IS61C3216     | SRAM 32K x 16       | 1         |
| E28F400B5     | Flash 256K x 16     | 1         |
| AM29F002      | Flash 256K x 8      | 2         |

</details>

## Board details

### General
- The 8086 itself.
- 128kB SRAM + 512kB flash.
- Logic for address latching (including OR gate with bus ALE).
- Logic for generating memory segment CS signals.
- Logic for generating IO device CS signals for first 8 16-bit devices - IO[0-7].
- Expansion port allowing access to 8086 bus.
This may be used for future developments - perhapse an FPGA graphics card with HDMI output.
- LED indicators for: latched adderss, memory access (SRAM 1 and SRAM 2, Flash is A19).
- Power LED.
- Reset button.
- USB-C port for power and communication with coprocessor.


### IO
- RGB LCD socket.
- Character LCD connector.
- 32 Push Buttons.  
They form a keyboard, with 'a'-'z', space, enter, backspace, CTRL, ALT, SHIFT.
- 16 LEDs.
- Two 7-segment displays.
- Logic to latch data to LEDs and from buttons.


### Co-Processor
- Mircocontroller used to control the 8086 (generating clock and reset signals), programming the memory and generating interrupts.
- Clock source: if I dont want to generate 33% duty cycle, than maybe a lower frequency 50% duty cycle instead.
  (make 66% of 8MHz the 50% of frequency - which is 6MHz).
- Maybe also act as peripherals (with or without interrupts): timers, UART, mouse (input from touch screen), SD card interface, etc...
- Resistvive touch screen connector.
- USB to UART convertor (SILABS CP2102).
- USB-C port for power and to connect to computer.
- UART connector (header pins), making CP2102 IC optional .


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
| 0x0_0000 - 0x4_0000   | 0 - 3     | 256kB | SRAM                              |
| 0x2_0000 - 0x6_0000   | 4 - 5     | 128kB | FREE                              |
| 0x6_0000 - 0x7_0000   | 6         | 64 kB | MMIO - Microcontroller            |
| 0x7_0000 - 0x7_8000   | 7         | 32 kB | MMIO - RGB LCD - DATA             |
| 0x7_8000 - 0x8_0000   | 7         | 32 kB | MMIO - RGB LCD  - COMMAND         |
| 0x8_0000 - 0xF_FFFF   | 8 - 15    | 512kB | Flash                             |

### IO map

IO ports are 16-bit wide.

| Number    | Address   | Read description | Write description    |
| --------- | --------- | ---------------- | -------------------- |
| 0         | 0x0000    | 16 Push buttons. | 2 7-segment display. |
| 1         | 0x0002    | 16 Push buttons. | 2 7-segment display. |
| 2         | 0x0004    | FREE             | FREE                 |
| 3         | 0x0006    | FREE             | FREE                 |
| 4         | 0x0008    | FREE             | FREE                 |
| 5         | 0x000A    | FREE             | FREE                 |
| 6         | 0x000C    | FREE             | FREE                 |
| 7         | 0x000E    | FREE             | FREE                 |

</details>