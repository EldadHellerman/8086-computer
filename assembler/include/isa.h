#ifndef OPCODES_H
#define  OPCODES_H

#include "inttypes.h"

// Registers:

typedef struct{
    const char *name;
    uint8_t binary;
} reg;
extern reg registers[];
extern uint16_t number_of_registers;



// Flags:

#define FLAGS_MASK_OVERFLOW             (1 << 11)
#define FLAGS_MASK_DIRECTION            (1 << 10)
#define FLAGS_MASK_INTERRUPT_ENABLE     (1 <<  9)
#define FLAGS_MASK_TRAP                 (1 <<  8)
#define FLAGS_MASK_SIGN                 (1 <<  7)
#define FLAGS_MASK_ZERO                 (1 <<  6)
#define FLAGS_MASK_AUXILIARY_CARRY      (1 <<  4)
#define FLAGS_MASK_PARITY               (1 <<  2)
#define FLAGS_MASK_CARRY                (1 <<  0)



// Mnemonics:
// prefixes are: LOCK, REP, Segment Override, then instruction.
// (original 8086 ASM generated them in this order).

typedef struct{

} binary_template;

typedef struct{
    const char *mnemonic;
    // maybe pointer to parsing function?
    int size;
    char b;
} instruction;

// 116 strings
// 88 instrcutions
extern instruction instructions[];
extern uint16_t number_of_instructions;

#endif