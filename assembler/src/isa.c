#include "isa.h"

// Registers:

/* {name, binary} */
reg registers_word[] = {
    {"AX", 0},
    {"CX", 1},
    {"DX", 2},
    {"BX", 3},
    {"SP", 4},
    {"BP", 5},
    {"SI", 6},
    {"DI", 7}
};
uint8_t registers_word_length = sizeof(registers_word) / sizeof(reg);

reg registers_byte[] = {
    {"AL", 0},
    {"CL", 1},
    {"DL", 2},
    {"BL", 3},
    {"AH", 4},
    {"CH", 5},
    {"DH", 6},
    {"BH", 7},
};
uint8_t registers_byte_length = sizeof(registers_byte) / sizeof(reg);

reg registers_segment[] = {
    {"ES", 0},
    {"CS", 1},
    {"SS", 2},
    {"DS", 3},
};
uint8_t registers_segment_length = sizeof(registers_segment) / sizeof(reg);



// Instructions:

instruction instructions[] = {
    // Data Transfer:
    {"MOV", },    // Move
    {"PUSH", },   // Push
    {"POP", },    // Pop
    {"XCHG", },   // Exchange
    {"NOP", },    // Nop - same as XCHG AX
    {"IN", },     // Input
    {"OUT", },    // Output
    {"XLAT", },   // Translate byte
    {"LEA", },    // Load effective address
    {"LDS", },    // Load to DS
    {"LES", },    // Load to ES
    {"LAHF", },   // Load AH with flags
    {"SAHF", },   // Store AH with flags
    {"PUSHF", },  // Push flags
    {"POPF", },   // Pop flags
    
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
    {"SAL", }, // same as SHL
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
    {"REPE", }, //same as REP
    {"REPZ", }, //same as REP
    {"REPNE", },
    {"REPNZ", }, //same as REPNE
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
    {"JZ", }, // same as JE
    {"JL", },
    {"JNGE", }, // same as JL
    {"JLE", },
    {"JNG", }, // same as JLE
    {"JB", },
    {"JNAE", }, // same as JB
    {"JC", }, // same as JB
    {"JBE", },
    {"JNA", }, // same as JBE
    {"JP", },
    {"JPE", }, // same as JP
    {"JO", },
    {"JS", },
    {"JNE", },
    {"JNZ", }, // same as JNE
    {"JNL", },
    {"JGE", }, // same as JNL
    {"JNLE", },
    {"JG", }, // same as JNLE
    {"JNB", },
    {"JAE", }, // same as JNB
    {"JNC", }, // same as JNB
    {"JNBE", },
    {"JA", }, // same as JNBE
    {"JNP", },
    {"JPO", }, // same as JNP
    {"JNO", },
    {"JNS", },
    
    {"LOOP", },
    {"LOOPZ", },
    {"LOOPE", }, // same as LOOPZ
    {"LOOPNZ", },
    {"LOOPNE", }, // same as LOOPNZ
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
    {"LOCK", },
};
uint16_t number_of_instructions = sizeof(instructions) / sizeof(instruction);

