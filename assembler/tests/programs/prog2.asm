// preprocessor

#ifndef _PROG_2_H
#define _PROG_2_H
#include "RGB_LCD.asm"
#define num     5

#define color565(r,g,b) 0x0000 | (((r&0x00f8) << 8) | ((g&0x00fc) << 3) | ((b&0x00f8) >> 3))

#macro draw_rect(first, second, x1, x2, x3)
    mov dx, first
    pushw second
    pushw x1
    pushw (x2)-1
    pushw (x3)-1
    call rgblcd_draw_rect
    add sp, 8
#endmacro

#if 4 < 5
mov ax, 5
#endif

#endif
