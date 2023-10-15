#include "assembler.h"

// Directives:

const char *strings_directives[] = {
    ".SECTION", // .section  SECTION_NAME
    ".GLOBAL", // .global LABEL_NAME
    ".EXTERN", // .extern LABEL_NAME, or like gcc, every not defiend label is extern by default
    ".ENTRY", // .entry  LABEL_NAME, or just .entry for current location
    ".TIMES", // .times number ... \n
    ".EQU", // .equ identifier expression, like ".equ msg_len $-message"

    ".INCBIN", // .incbin "FILE_PATH"
    ".ALIGN", // .align number
    ".DB", // .db list of 8-bit integers, chars, or strings
    ".DW", // .dw list of 16-bit integers.
    ".DD", // .dd list of 32-bit integers, or floating point numbers.
    ".DQ", // .dq list of 64-bit integers, or double floating point numbers.

    ".ORG", // .org origin_address
    ".RESB", // .resb number_of_1_bytes
    ".RESW", // .resw number_of_2_bytes
    ".RESD", // .resd number_of_4_bytes
    ".RESQ", // .resq number_of_8_bytes
    
};
uint8_t strings_directives_length = sizeof(strings_directives) / sizeof(char*);
