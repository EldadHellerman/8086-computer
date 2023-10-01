#include "isa.h"

// Registers:

/* {name, binary} */
reg_t registers_word[] = {
    {"AX", 0},
    {"CX", 1},
    {"DX", 2},
    {"BX", 3},
    {"SP", 4},
    {"BP", 5},
    {"SI", 6},
    {"DI", 7}
};
uint8_t registers_word_length = sizeof(registers_word) / sizeof(reg_t);

reg_t registers_byte[] = {
    {"AL", 0},
    {"CL", 1},
    {"DL", 2},
    {"BL", 3},
    {"AH", 4},
    {"CH", 5},
    {"DH", 6},
    {"BH", 7},
};
uint8_t registers_byte_length = sizeof(registers_byte) / sizeof(reg_t);

reg_t registers_segment[] = {
    {"ES", 0},
    {"CS", 1},
    {"SS", 2},
    {"DS", 3},
};
uint8_t registers_segment_length = sizeof(registers_segment) / sizeof(reg_t);



// Instructions:

instruction_t instructions[] = {
    {"MOV", 6, (if_t *)&(if_t[])
        {{IF_DSV_D | IF_FLAG_W | IF_BYTES_MOD_RM | IF_FLAG_REG, 0b10001000, 0},
         {IF_FLAG_W | IF_BYTES_MOD_RM | IF_FLAG_DATA, 0b11000110, 0},
         {IF_FLAG_W | IF_FLAG_REG | IF_FLAG_DATA, 0b10110000, 0},
         {IF_FLAG_W | IF_IMPLICIT_AX_DST | IF_FLAG_ADDR16, 0b10100000, 0},
         {IF_FLAG_W | IF_IMPLICIT_AX_SRC | IF_FLAG_ADDR16, 0b10100010, 0},
         {IF_DSV_D | IF_BYTES_MOD_RM | IF_FLAG_SREG, 0b10001100, 0}}},
    {"PUSH", 3, (if_t *)&(if_t[])
        {{IF_BYTES_MOD_RM, 0b11111111, 0b00110000},
         {IF_FLAG_REG, 0b01010000, 0},
         {IF_FLAG_SREG, 0b00000110, 0}}},
    {"POP", 3, (if_t *)&(if_t[])
        {{IF_BYTES_MOD_RM, 0b10001111, 0},
         {IF_FLAG_REG, 0b01011000, 0},
         {IF_FLAG_SREG, 0b00000111, 0}}},
    {"XCHG", 2, (if_t *)&(if_t[])
        {{IF_FLAG_W | IF_BYTES_MOD_RM | IF_FLAG_REG, 0b10000110, 0},
         {IF_FLAG_REG | IF_IMPLICIT_AX_SRC | IF_IMPLICIT_AX_DST, 0b10010000, 0}}},
    {"NOP", 1, (if_t *)&(if_t[]){{IF_BYTES_ONE, 0b10011001, 0}}},
    {"IN", 2, (if_t *)&(if_t[])
        {{IF_FLAG_W | IF_IMPLICIT_AX_DST | IF_FLAG_DATA8, 0b11100100, 0},
         {IF_FLAG_W | IF_IMPLICIT_AX_DST | IF_IMPLICIT_DX_SRC, 0b11101100, 0}}},
    {"OUT", 2, (if_t *)&(if_t[])
        {{IF_FLAG_W | IF_IMPLICIT_AX_SRC | IF_FLAG_DATA8, 0b11100110, 0},
         {IF_FLAG_W | IF_IMPLICIT_AX_SRC | IF_IMPLICIT_DX_DST, 0b11101110, 0}}},
    {"XLAT",    1, (if_t *)&(if_t[]){{IF_BYTES_ONE, 0b11010111, 0}}},
    {"LEA",     1, (if_t *)&(if_t[]){{IF_BYTES_MOD_RM | IF_FLAG_REG, 0b10001101, 0}}},
    {"LDS",     1, (if_t *)&(if_t[]){{IF_BYTES_MOD_RM | IF_FLAG_REG, 0b11000101, 0}}},
    {"LES",     1, (if_t *)&(if_t[]){{IF_BYTES_MOD_RM | IF_FLAG_REG, 0b11000100, 0}}},
    {"LAHF",    1, (if_t *)&(if_t[]){{IF_BYTES_ONE, 0b10011111, 0}}},
    {"SAHF",    1, (if_t *)&(if_t[]){{IF_BYTES_ONE, 0b10011110, 0}}},
    {"PUSHF",   1, (if_t *)&(if_t[]){{IF_BYTES_ONE, 0b10011100, 0}}},
    {"POPF",    1, (if_t *)&(if_t[]){{IF_BYTES_ONE, 0b10011101, 0}}},
    {"ADD", 3, (if_t *)&(if_t[])
        {{IF_DSV_D | IF_FLAG_W  | IF_BYTES_MOD_RM | IF_FLAG_REG, 0, 0},
         {IF_DSV_S | IF_FLAG_W | IF_BYTES_MOD_RM | IF_FLAG_DATA, 0b10000000, 0},
         {IF_FLAG_W | IF_IMPLICIT_AX_DST | IF_FLAG_DATA, 0b00000100, 0}}},
    {"ADC", 3, (if_t *)&(if_t[])
        {{IF_DSV_D | IF_FLAG_W | IF_BYTES_MOD_RM | IF_FLAG_REG, 0b00010000, 0},
         {IF_DSV_S | IF_FLAG_W | IF_BYTES_MOD_RM | IF_FLAG_DATA, 0b10000000, 0b00010000},
         {IF_FLAG_W | IF_IMPLICIT_AX_DST | IF_FLAG_DATA, 0b00010100, 0}}},
    {"INC", 2, (if_t *)&(if_t[])
        {{IF_FLAG_W | IF_BYTES_MOD_RM, 0b11111110, 0},
         {IF_FLAG_REG, 0b01000000, 0}}},
    {"AAA", 1, (if_t *)&(if_t[]){{IF_BYTES_ONE, 0b00110111, 0}}},
    {"DAA", 1, (if_t *)&(if_t[]){{IF_BYTES_ONE, 0b00100111, 0}}},
    {"SUB", 3, (if_t *)&(if_t[])
        {{IF_DSV_D | IF_FLAG_W | IF_BYTES_MOD_RM | IF_FLAG_REG, 0b00101000, 0},
         {IF_DSV_S | IF_FLAG_W | IF_BYTES_MOD_RM | IF_FLAG_DATA, 0b10000000, 0b00101000},
         {IF_FLAG_W | IF_IMPLICIT_AX_DST | IF_FLAG_DATA, 0b00101100, 0}}},
    {"SBB", 3, (if_t *)&(if_t[])
        {{IF_DSV_D | IF_FLAG_W | IF_BYTES_MOD_RM | IF_FLAG_REG, 0b00011000, 0},
         {IF_DSV_S | IF_FLAG_W | IF_BYTES_MOD_RM | IF_FLAG_DATA, 0b10000000, 0b00011000},
         {IF_FLAG_W | IF_IMPLICIT_AX_DST | IF_FLAG_DATA, 0b00011100, 0}}},
    {"DEC", 2, (if_t *)&(if_t[])
        {{IF_FLAG_W | IF_BYTES_MOD_RM, 0b11111110, 0b00001000},
         {IF_FLAG_REG, 0b01001000, 0}}},
    {"NEG", 1, (if_t *)&(if_t[]){{IF_FLAG_W | IF_BYTES_MOD_RM, 0b11110110, 0b00011000}}},
    {"CMP", 3, (if_t *)&(if_t[])
        {{IF_DSV_D | IF_FLAG_W | IF_BYTES_MOD_RM | IF_FLAG_REG, 0b00111000, 0},
         {IF_DSV_S | IF_FLAG_W | IF_BYTES_MOD_RM | IF_FLAG_DATA, 0b10000000, 0b00111000},
         {IF_FLAG_W | IF_IMPLICIT_AX_DST | IF_FLAG_DATA, 0b00111100, 0}}},
    {"AAS",     1, (if_t *)&(if_t[]){{IF_BYTES_ONE, 0b00111111, 0}}},
    {"DAS",     1, (if_t *)&(if_t[]){{IF_BYTES_ONE, 0b00101111, 0}}},
    {"MUL",     1, (if_t *)&(if_t[]){{IF_FLAG_W | IF_BYTES_MOD_RM, 0b11110110, 0b00100000}}},
    {"IMUL",    1, (if_t *)&(if_t[]){{IF_FLAG_W | IF_BYTES_MOD_RM, 0b11110110, 0b00101000}}},
    {"AAM",     1, (if_t *)&(if_t[]){{IF_BYTES_TWO, 0b11110110, 0b00101000}}},
    {"DIV",     1, (if_t *)&(if_t[]){{IF_FLAG_W | IF_BYTES_MOD_RM, 0b11110110, 0b00110000}}},
    {"IDIV",    1, (if_t *)&(if_t[]){{IF_FLAG_W | IF_BYTES_MOD_RM, 0b11110110, 0b00111000}}},
    {"AAD",     1, (if_t *)&(if_t[]){{IF_BYTES_TWO, 0b11110110, 0b00101000}}},
    {"CBW",     1, (if_t *)&(if_t[]){{IF_BYTES_ONE, 0b10011000, 0}}},
    {"CWD",     1, (if_t *)&(if_t[]){{IF_BYTES_ONE, 0b10011001, 0}}},
    {"NOT",     1, (if_t *)&(if_t[]){{IF_FLAG_W | IF_BYTES_MOD_RM, 0b11110110, 0b00010000}}},
    {"SHL",     1, (if_t *)&(if_t[]){{IF_DSV_V | IF_FLAG_W | IF_BYTES_MOD_RM, 0b11010000, 0b00100000}}},
    {"SHR",     1, (if_t *)&(if_t[]){{IF_DSV_V | IF_FLAG_W | IF_BYTES_MOD_RM, 0b11010000, 0b00101000}}},
    {"SAR",     1, (if_t *)&(if_t[]){{IF_DSV_V | IF_FLAG_W | IF_BYTES_MOD_RM, 0b11010000, 0b00111000}}},
    {"ROL",     1, (if_t *)&(if_t[]){{IF_DSV_V | IF_FLAG_W | IF_BYTES_MOD_RM, 0b11010000, 0b00000000}}},
    {"ROR",     1, (if_t *)&(if_t[]){{IF_DSV_V | IF_FLAG_W | IF_BYTES_MOD_RM, 0b11010000, 0b00001000}}},
    {"RCL",     1, (if_t *)&(if_t[]){{IF_DSV_V | IF_FLAG_W | IF_BYTES_MOD_RM, 0b11010000, 0b00010000}}},
    {"RCR",     1, (if_t *)&(if_t[]){{IF_DSV_V | IF_FLAG_W | IF_BYTES_MOD_RM, 0b11010000, 0b00011000}}},
    {"AND", 3, (if_t *)&(if_t[])
        {{IF_DSV_D | IF_FLAG_W | IF_BYTES_MOD_RM | IF_FLAG_REG, 0b00100000, 0},
         {IF_FLAG_W | IF_BYTES_MOD_RM | IF_FLAG_DATA, 0b10000000, 0b00100000},
         {IF_FLAG_W | IF_IMPLICIT_AX_DST | IF_FLAG_DATA, 0b00100100, 0}}},
    {"TEST", 3, (if_t *)&(if_t[])
        {{IF_FLAG_W | IF_BYTES_MOD_RM | IF_FLAG_REG, 0b10000100, 0},
         {IF_FLAG_W | IF_BYTES_MOD_RM | IF_FLAG_DATA, 0b11110110, 0b00000000},
         {IF_FLAG_W | IF_IMPLICIT_AX_DST | IF_IMPLICIT_AX_SRC | IF_FLAG_DATA, 0b10101000, 0}}},
    {"OR", 3, (if_t *)&(if_t[])
        {{IF_DSV_D | IF_FLAG_W | IF_BYTES_MOD_RM | IF_FLAG_REG, 0b00001000, 0},
         {IF_FLAG_W | IF_BYTES_MOD_RM | IF_FLAG_DATA, 0b10000000, 0b00001000},
         {IF_FLAG_W | IF_IMPLICIT_AX_DST | IF_FLAG_DATA, 0b00001100, 0}}},
    {"XOR", 3, (if_t *)&(if_t[])
        {{IF_DSV_D | IF_FLAG_W | IF_BYTES_MOD_RM | IF_FLAG_REG, 0b00110000, 0},
         {IF_FLAG_W | IF_BYTES_MOD_RM | IF_FLAG_DATA, 0b10000000, 0b00110000},
         {IF_FLAG_W | IF_IMPLICIT_AX_DST | IF_FLAG_DATA, 0b00110100, 0}}},
    {"REP",     1, (if_t *)&(if_t[]){{IF_BYTES_ONE, 0b11110011, 0}}},
    {"REPNE",   1, (if_t *)&(if_t[]){{IF_BYTES_ONE, 0b11110010, 0}}},
    {"MOVSB",   1, (if_t *)&(if_t[]){{IF_BYTES_ONE, 0b10100100, 0}}},
    {"MOVSW",   1, (if_t *)&(if_t[]){{IF_BYTES_ONE, 0b10100101, 0}}},
    {"CMPSB",   1, (if_t *)&(if_t[]){{IF_BYTES_ONE, 0b10100110, 0}}},
    {"CMPSW",   1, (if_t *)&(if_t[]){{IF_BYTES_ONE, 0b10100111, 0}}},
    {"SCASB",   1, (if_t *)&(if_t[]){{IF_BYTES_ONE, 0b10101110, 0}}},
    {"SCASW",   1, (if_t *)&(if_t[]){{IF_BYTES_ONE, 0b10101111, 0}}},
    {"LODSB",   1, (if_t *)&(if_t[]){{IF_BYTES_ONE, 0b10101100, 0}}},
    {"LODSW",   1, (if_t *)&(if_t[]){{IF_BYTES_ONE, 0b10101101, 0}}},
    {"STOSB",   1, (if_t *)&(if_t[]){{IF_BYTES_ONE, 0b10101010, 0}}},
    {"STOSW",   1, (if_t *)&(if_t[]){{IF_BYTES_ONE, 0b10101011, 0}}},
    {"CALL", 4, (if_t *)&(if_t[])
        {{IF_BYTES_ONE | IF_FLAG_DISP16, 0b11101000, 0},
         {IF_BYTES_MOD_RM, 0b11111111, 0b00010000},
         {IF_BYTES_ONE | IF_FLAG_ADDR16 | IF_FLAG_SEG16, 0b10011010, 0},
         {IF_BYTES_MOD_RM, 0b11111111, 0b00011000}}},
    
    {"JMP", 5, (if_t *)&(if_t[])
        {{IF_BYTES_ONE | IF_FLAG_DISP16, 0b11101001, 0},
         {IF_BYTES_ONE | IF_FLAG_DISP8, 0b11101011, 0},
         {IF_BYTES_MOD_RM, 0b11111111, 0b00100000},
         {IF_BYTES_ONE | IF_FLAG_ADDR16 | IF_FLAG_SEG16, 0b11101010, 0},
         {IF_BYTES_MOD_RM, 0b11111111, 0b00101000}}},
    {"RET", 2, (if_t *)&(if_t[])
        {{IF_BYTES_ONE, 0b11000011, 0},
         {IF_BYTES_ONE | IF_FLAG_DATA, 0b11000010, 0}}},
    {"RETF", 2, (if_t *)&(if_t[])
        {{IF_BYTES_ONE, 0b10001011, 0},
         {IF_BYTES_ONE | IF_FLAG_DATA, 0b11001010, 0}}},
    {"JE",      1, (if_t *)&(if_t[]){{IF_FLAG_DISP8, 0b01110100, 0}}},
    {"JL",      1, (if_t *)&(if_t[]){{IF_FLAG_DISP8, 0b01111100, 0}}},
    {"JLE",     1, (if_t *)&(if_t[]){{IF_FLAG_DISP8, 0b01111110, 0}}},
    {"JB",      1, (if_t *)&(if_t[]){{IF_FLAG_DISP8, 0b01110010, 0}}},
    {"JBE",     1, (if_t *)&(if_t[]){{IF_FLAG_DISP8, 0b01110110, 0}}},
    {"JP",      1, (if_t *)&(if_t[]){{IF_FLAG_DISP8, 0b01111010, 0}}},
    {"JO",      1, (if_t *)&(if_t[]){{IF_FLAG_DISP8, 0b01110000, 0}}},
    {"JS",      1, (if_t *)&(if_t[]){{IF_FLAG_DISP8, 0b01111000, 0}}},
    {"JNE",     1, (if_t *)&(if_t[]){{IF_FLAG_DISP8, 0b01110101, 0}}},
    {"JNL",     1, (if_t *)&(if_t[]){{IF_FLAG_DISP8, 0b01111101, 0}}},
    {"JNLE",    1, (if_t *)&(if_t[]){{IF_FLAG_DISP8, 0b01111111, 0}}},
    {"JNB",     1, (if_t *)&(if_t[]){{IF_FLAG_DISP8, 0b01110011, 0}}},
    {"JNBE",    1, (if_t *)&(if_t[]){{IF_FLAG_DISP8, 0b01110111, 0}}},
    {"JNP",     1, (if_t *)&(if_t[]){{IF_FLAG_DISP8, 0b01111011, 0}}},
    {"JNO",     1, (if_t *)&(if_t[]){{IF_FLAG_DISP8, 0b01110001, 0}}},
    {"JNS",     1, (if_t *)&(if_t[]){{IF_FLAG_DISP8, 0b01111001, 0}}},
    {"LOOP",    1, (if_t *)&(if_t[]){{IF_FLAG_DISP8, 0b11100001, 0}}},
    {"LOOPZ",   1, (if_t *)&(if_t[]){{IF_FLAG_DISP8, 0b11100001, 0}}},
    {"LOOPNZ",  1, (if_t *)&(if_t[]){{IF_FLAG_DISP8, 0b11100000, 0}}},
    {"JCXZ",    1, (if_t *)&(if_t[]){{IF_FLAG_DISP8, 0b11100011, 0}}},
    {"INT",     2, (if_t *)&(if_t[])
        {{IF_FLAG_DATA8, 0b11001101, 0},
         {IF_IMPLICIT_0x03, 0b11001100, 0}}},
    {"INTO",     1, (if_t *)&(if_t[]){{IF_BYTES_ONE, 0b11001110, 0}}},
    {"IRET",     1, (if_t *)&(if_t[]){{IF_BYTES_ONE, 0b11001111, 0}}},
    {"CLC",     1, (if_t *)&(if_t[]){{IF_BYTES_ONE, 0b11111000, 0}}},
    {"CMC",     1, (if_t *)&(if_t[]){{IF_BYTES_ONE, 0b11110101, 0}}},
    {"STC",     1, (if_t *)&(if_t[]){{IF_BYTES_ONE, 0b11111001, 0}}},
    {"CLD",     1, (if_t *)&(if_t[]){{IF_BYTES_ONE, 0b11111100, 0}}},
    {"STD",     1, (if_t *)&(if_t[]){{IF_BYTES_ONE, 0b11111101, 0}}},
    {"CLI",     1, (if_t *)&(if_t[]){{IF_BYTES_ONE, 0b11111010, 0}}},
    {"STI",     1, (if_t *)&(if_t[]){{IF_BYTES_ONE, 0b11111011, 0}}},
    {"HLT",     1, (if_t *)&(if_t[]){{IF_BYTES_ONE, 0b11110100, 0}}},
    {"WAIT",    1, (if_t *)&(if_t[]){{IF_BYTES_ONE, 0b10011011, 0}}},
    {"ESC",     1, (if_t *)&(if_t[]){{IF_FLAG_ESC | IF_BYTES_MOD_RM, 0b11011000, 0}}},
    {"LOCK",    1, (if_t *)&(if_t[]){{IF_BYTES_ONE, 0b11110000, 0}}}
};
uint16_t instructions_length = sizeof(instructions) / sizeof(instruction_t);


instruction_synonym_t instructions_synonyms[] = {
    {"SAL", "SHL"},
    {"REPE", "REP"},
    {"REPZ", "REP"},
    {"REPNZ", "REPNE"},
    {"JZ", "JE"},
    {"JNGE", "JL"},
    {"JNG", "JLE"},
    {"JNAE", "JB"},
    {"JC", "JB"},
    {"JNA", "JBE"},
    {"JPE", "JP"},
    {"JNZ", "JNE"},
    {"JGE", "JNL"},
    {"JG", "JNLE"},
    {"JAE", "JNB"},
    {"JNC", "JNB"},
    {"JA", "JNBE"},
    {"JPO", "JNP"},
    {"LOOPE", "LOOPZ"},
    {"LOOPNE", "LOOPNZ"},
};
uint16_t instructions_synonyms_length = sizeof(instructions_synonyms) / sizeof(instruction_synonym_t);

