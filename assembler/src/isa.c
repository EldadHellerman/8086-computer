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
    // Data Transfer:
    {"MOV", 6, (if_t *)&(if_t[])
        {{IF_FLAG_D | IF_FLAG_W | IF_FLAG_MOD_RM | IF_FLAG_REG, 0b10001000, 0},
         {IF_FLAG_W | IF_FLAG_MOD_RM | IF_FLAG_DATA, 0b11000110, 0},
         {IF_FLAG_W | IF_FLAG_REG | IF_FLAG_DATA, 0b10110000, 0},
         {IF_FLAG_W | IF_FLAG_AX_DST | IF_FLAG_ADDR16, 0b10100000, 0},
         {IF_FLAG_W | IF_FLAG_AX_SRC | IF_FLAG_ADDR16, 0b10100010, 0},
         {IF_FLAG_D | IF_FLAG_MOD_RM | IF_FLAG_REG, 0b10001100, 0}}},
    {"PUSH", 3, (if_t *)&(if_t[])
        {{IF_FLAG_MOD_RM, 0b11111111, 0b0011000},
         {IF_FLAG_REG, 0b01010000, 0},
         {IF_FLAG_SREG, 0b00000110, 0}}},
    {"POP", 3, (if_t *)&(if_t[])
        {{IF_FLAG_MOD_RM, 0b10001111, 0},
         {IF_FLAG_REG, 0b01011000, 0},
         {IF_FLAG_SREG, 0b00000111, 0}}},
    {"XCHG", 2, (if_t *)&(if_t[])
        {{IF_FLAG_W | IF_FLAG_MOD_RM | IF_FLAG_REG, 0b10000110, 0},
         {IF_FLAG_REG | IF_FLAG_AX_SRC | IF_FLAG_AX_DST, 0b10010000, 0}}},
    {"NOP", 1, (if_t *)&(if_t[]){{IF_FLAG_MOD_RM, 0b10011001, 0}}},
    {"IN", 2, (if_t *)&(if_t[])
        {{IF_FLAG_W | IF_FLAG_AX_DST | IF_FLAG_DATA8, 0b11100100, 0},
         {IF_FLAG_W | IF_FLAG_AX_DST | IF_FLAG_DX, 0b11101100, 0}}},
    {"OUT", 2, (if_t *)&(if_t[])
        {{IF_FLAG_W | IF_FLAG_AX_SRC | IF_FLAG_DATA8, 0b11100110, 0},
         {IF_FLAG_W | IF_FLAG_AX_SRC | IF_FLAG_DX, 0b11101110, 0}}},
    {"XLAT", 1, (if_t *)&(if_t[]){{0, 0b11010111, 0}}},
    {"LEA", 1, (if_t *)&(if_t[]){{IF_FLAG_MOD_RM | IF_FLAG_REG, 0b10001101, 0}}},
    {"LDS", 1, (if_t *)&(if_t[]){{IF_FLAG_MOD_RM | IF_FLAG_REG, 0b11000101, 0}}},
    {"LES", 1, (if_t *)&(if_t[]){{IF_FLAG_MOD_RM | IF_FLAG_REG, 0b11000100, 0}}},
    {"LAHF", 1, (if_t *)&(if_t[]){{0, 0b10011111, 0}}},
    {"SAHF", 1, (if_t *)&(if_t[]){{0, 0b10011110, 0}}},
    {"PUSHF", 1, (if_t *)&(if_t[]){{0, 0b10011100, 0}}},
    {"POPF", 1, (if_t *)&(if_t[]){{0, 0b10011101, 0}}},
    /*
    // Arithmetic:
    {"ADD", },
    {"ADC", },
    {"INC", },
    {"AAA", },
    {"DAA", },
    {"SUB", },
    {"SBB", },
    {"DEC", },
    {"NEG", },
    {"CMP", },
    {"AAS", },
    {"DAS", },
    {"MUL", },
    {"IMUL", },
    {"AAM", },
    {"DIV", },
    {"IDIV", },
    {"AAD", },
    {"CBW", },
    {"CWD", },

    // Logic:
    {"NOT", },
    {"SHL", },
    {"SHR", },
    {"SAR", },
    {"ROL", },
    {"ROR", },
    {"RCL", },
    {"RCR", },
    {"AND", },
    {"TEST", },
    {"OR", },
    {"XOR", },

    // String manipulation:
    {"REP", },
    {"REPNE", },
    {"MOVSB", },
    {"MOVSW", },
    {"CMPSB", },
    {"CMPSW", },
    {"SCASB", },
    {"SCASW", },
    {"LODSB", },
    {"LODSW", },
    {"STOSB", },
    {"STOSW", },

    // Control transfer:
    {"CALL", },
    {"JMP", },
    {"RET", }, // return near
    {"RETF", }, // return far
    {"JE", },
    {"JL", },
    {"JLE", },
    {"JB", },
    {"JBE", },
    {"JP", },
    {"JO", },
    {"JS", },
    {"JNE", },
    {"JNL", },
    {"JNLE", },
    {"JNB", },
    {"JNBE", },
    {"JNP", },
    {"JNO", },
    {"JNS", },
    
    {"LOOP", },
    {"LOOPZ", },
    {"LOOPNZ", },
    {"JCXZ", },

    {"INT", },
    {"INTO", },
    {"IRET", },

    // Processor control:
    {"CLC", },
    {"CMC", },
    {"STC", },
    {"CLD", },
    {"STD", },
    {"CLI", },
    {"STI", },
    {"HLT", },
    {"WAIT", },
    {"ESC", },
    {"LOCK", },*/
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

