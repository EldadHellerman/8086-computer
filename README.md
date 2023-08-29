# 8086

New version of the 8086 computer (moved to repository "8086 computer_OLD"), mainly focusing on PCB design.

## Available Components
I'm trying to recycle old components I salvaged and not buy anything new (excpet for the 8086 itself which I didn't have).

These are the IC's I have lying around:

### Logic

| Name          | Description                               | amount    |
| ------------- | ----------------------------------------- | --------- |
| 74AC373       | Octal latch with 3-STATE Outputs          | 20+       |
| 74HC138D      | 3 to 8 line decoder; inverting            | 5         |
| 74HC132       | Quad 2-input NAND gate; Schmitt trigger   | 6         |
| 74AC00        | Quad 2-input NAND gate                    | 2         |
| 74HC02        | Quad 2-input NOR gate                     | 5         |
| 74ACT08       | Quad 2-input AND gate                     | 1         |
| HEF40106      | Hex inverting Schmitt trigger             | 1         |
| 74HC14        | Hex inverting Schmitt trigger             | 1         |

### Memory

| Name          | Description           | amount    |
| ------------- | --------------------- | --------- |
| IS61C256AH    |	SRAM 32K x 8        | 5         |
| CXK58257AM    |	SRAM 32K x 8        | 1         |
| IS61C3216     |	SRAM 32K x 16       | 1         |
| E28F400B5     |	Flash 256K x 16     | 1         |
| AM29F002      |	Flash 256K x 8      | 2         |




## Address Space

### Segments and physical address

The 8086 uses 20-bit addressing (for 1MB total).  
The physical address is calculated from a 16-bit segment register and a 16-bit address register:

```
physical address = (segment << 4) + offset
```

For convenience, I will think of each segment as being only the 4 highest order bits (0x?000), and the memory as being comprised of 16 segments, each 64kB in size.
    
For in / out instructions, only 16-bits addressing is allowed (first segment only).

The 8086 boots to address 0xFFFF0, so there must be some flash there. That's why the flash is at the top of the memory map.

### Memory map:

| Addresses             | Size  | Description               |
| --------------------- | ----- | ------------------------- |
| 0x0_0000 - 0x2_0000   | 128kB | SRAM                      |
| 0x6_0000 - 0x7_0000   | 32kB  | MMIO - Microcontroller    |
| 0x7_0000 - 0x7_8000   | 32kB  | MMIO - RGB LCD - DATA     |
| 0x7_8000 - 0x8_0000   | 32kB  | MMIO - RGB LCD  - COMMAND |
| 0x8_0000 - 0xF_FFFF   | 512kB | Flash                     |

### Segments map:

| Segments   | Description              |
| --------- | ------------------------- |
| 0 - 1     | SRAM                      |
| 2 - 5     | FREE                      |
| 6         | MMIO - Microcontroller    |
| 7         | MMIO - RGB LCD            |
| 8 - 15    | Flash                     |

### IO map:

| Address   | Description           |
| --------- | --------------------- |
| 0x0000    | Buttons with LED 1    |
| 0x0010    | LEDs 1                |
| 0x0020    | 7-Segment 1           |
| 0x0021    | 7-Segment 2           |
  
