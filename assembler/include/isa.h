#ifndef OPCODES_H
#define  OPCODES_H

#include <inttypes.h>
#include <stdbool.h>

/**
 * @brief 8086 ISA.
 * 
 * Bit locations are: (MSB) 15 14 13 12 11 10 9 8    7 6 5 4 3 2 1 0 (LSB).
 */


// Registers:

/** @struct reg
 *  @brief 8086 hardware register
 *  @param name Register name
 *  @param binary Register binary representation
 */
typedef struct{
    /** Register name */
    const char *name;
    /** Register binary representation */
    uint8_t binary;
} reg;

/** 8-bit registers array */
extern reg registers_byte[];
extern uint8_t registers_byte_length;
/** 16-bit registers array */
extern reg registers_word[];
extern uint8_t registers_word_length;
/** segment registers array */
extern reg registers_segment[];
extern uint8_t registers_segment_length;



// Flags:
#define FLAGS_OVERFLOW_BIT              11
#define FLAGS_DIRECTION_BIT             10
#define FLAGS_INTERRUPT_ENABLE_BIT      9
#define FLAGS_TRAP_BIT                  8
#define FLAGS_SIGN_BIT                  7
#define FLAGS_ZERO_BIT                  6
#define FLAGS_AUXILIARY_CARRY_BIT       4
#define FLAGS_PARITY_BIT                2
#define FLAGS_CARRY_BIT                 0

#define FLAGS_OVERFLOW_MASK             (1 << FLAGS_OVERFLOW_BIT)
#define FLAGS_DIRECTION_MASK            (1 << FLAGS_DIRECTION_BIT)
#define FLAGS_INTERRUPT_ENABLE_MASK     (1 << FLAGS_INTERRUPT_ENABLE_BIT)
#define FLAGS_TRAP_MASK                 (1 << FLAGS_TRAP_BIT)
#define FLAGS_SIGN_MASK                 (1 << FLAGS_SIGN_BIT)
#define FLAGS_ZERO_MASK                 (1 << FLAGS_ZERO_BIT)
#define FLAGS_AUXILIARY_CARRY_MASK      (1 << FLAGS_AUXILIARY_CARRY_BIT)
#define FLAGS_PARITY_MASK               (1 << FLAGS_PARITY_BIT)
#define FLAGS_CARRY_MASK                (1 << FLAGS_CARRY_BIT)



// Instructions formats:
/**
 * Instruction formats are divided into two - first byte and second byte (marked FB and SB).
 * 
 * XXX_FLAG indicate if this field is present for a given instruction.
 * XXX_BIT is the bit location of that field.
 * XXX_MASK is the mask for the field.
 */

/** Instruction format first byte 'w' field.
 * w = 0: Instruction operates on byte data.
 * w = 1: Instruction operates on word data.
 */
#define IF_FB_W_FLAG                    (1 <<  0)
#define IF_FB_W_WITH_REG_BIT            3
#define IF_FB_W_WITHOUT_REG_BIT         0
#define IF_FB_W_WITH_REG_MASK           (1 <<  IF_FB_W_WITH_REG_BIT)
#define IF_FB_W_WITHOUT_REG_MASK        (1 <<  IF_FB_W_WITHOUT_REG_BIT)

/** Instruction format first byte 'z' field.
 * This is used by REP instruction for string manipulation.
 * z = 0: Repeat while zero flag is clear.
 * z = 1: Repeat while zero flag is set.
 */
#define IF_FB_Z_FLAG                    (1 <<  1)
#define IF_FB_Z_BIT                     0
#define IF_FB_Z_MASK                    (1 << IF_FB_Z_BIT)

/** Instruction format first byte 'd' field.
 * d = 0: Repeat while zero flag is clear.
 * d = 1: Repeat while zero flag is set.
 */
#define IF_FB_D_FLAG                    (1 <<  2)
#define IF_FB_D_BIT                     1
#define IF_FB_D_MASK                    (1 << IF_FB_D_BIT)

/** Instruction format first byte 's' field.
 * s = 0: Do not sign extend - 16-bit immediate is given.
 * s = 1: Sign extend 8-bit immediate to 16 bits.
 */
#define IF_FB_S_FLAG                    (1 <<  3)
#define IF_FB_S_BIT                     1
#define IF_FB_S_MASK                    (1 << IF_FB_S_BIT)

/** Instruction format first byte 'v' field.
 * v = 0: Shift/rotate count is one.
 * v = 1: Shift/rotate count specified in CL register.
 */
#define IF_FB_V_FLAG                    (1 <<  4)
#define IF_FB_V_BIT                     1
#define IF_FB_V_MASK                    (1 << IF_FB_V_BIT)

/** Instruction format first byte 'reg' field.
 * reg is the register number.
 */
#define IF_FB_REG_FLAG                  (1 <<  5)
#define IF_FB_REG_BIT                   0
#define IF_FB_REG_MASK                  (0b111 << IF_FB_REG_BIT)

/** Instruction format first byte 'sreg' field.
 * sreg is the segment register number.
 */
#define IF_FB_SREG_FLAG                 (1 <<  6)
#define IF_FB_SREG_BIT                  3
#define IF_FB_SREG_MASK                 (0b11 << IF_FB_SREG_BIT)

#define IF_FB_XXX_FLAG          (1 <<  7)

// Instructions:
// prefixes are: LOCK, REP, Segment Override, then instruction.
// (original 8086 ASM generated them in this order).
// LOCK and REP are specified by the user with an insruction.
// segment override is part of the operands for an instruction.

typedef struct{

} binary_template;

typedef struct{
    const char *mnemonic;
    // maybe pointer to parsing function?
} instruction;

extern instruction instructions[];
extern uint16_t number_of_instructions;

#endif