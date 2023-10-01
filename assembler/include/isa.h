#ifndef ISA_H
#define  ISA_H

#include <inttypes.h>
#include <stdbool.h>

/**
 * @brief 8086 ISA.
 * 
 * Bit order in word and byte: (MSB) 15 14 13 12 11 10 9 8    7 6 5 4 3 2 1 0 (LSB).
 */


// Registers:

/** @struct reg_t
 *  @brief 8086 hardware register
 *  @param name Register name
 *  @param binary Register binary representation
 */
typedef struct{
    /** Register name */
    const char *name;
    /** Register binary representation */
    uint8_t binary;
} reg_t;

/** 8-bit registers array */
extern reg_t registers_byte[];
extern uint8_t registers_byte_length;
/** 16-bit registers array */
extern reg_t registers_word[];
extern uint8_t registers_word_length;
/** segment registers array */
extern reg_t registers_segment[];
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

/** Instruction format 'w' field.
 * w = 0: Instruction operates on byte data.
 * w = 1: Instruction operates on word data.
 */
#define IF_FLAG_W                       (1 <<  0)
//TODO make this better:
#define IF_FB_W_WITH_REG_BIT            3
#define IF_FB_W_WITHOUT_REG_BIT         0
#define IF_FB_W_WITH_REG_MASK           (1 <<  IF_FB_W_WITH_REG_BIT)
#define IF_FB_W_WITHOUT_REG_MASK        (1 <<  IF_FB_W_WITHOUT_REG_BIT)

/** Instruction format 'd' field.
 * d = 0: From register specified in 'reg' field ('reg' is source).
 * d = 1: To register specified in 'reg' field ('reg' is destination).
 */
#define IF_FLAG_D                       (1 <<  1)
#define IF_FB_D_BIT                     1
#define IF_FB_D_MASK                    (1 << IF_FB_D_BIT)

/** Instruction format 's' field.
 * s = 0: Do not sign extend - 16-bit immediate is given.
 * s = 1: Sign extend 8-bit immediate to 16 bits.
 */
#define IF_FLAG_S                       (1 <<  2)
#define IF_FB_S_BIT                     1
#define IF_FB_S_MASK                    (1 << IF_FB_S_BIT)

/** Instruction format 'v' field.
 * v = 0: Shift/rotate count is one.
 * v = 1: Shift/rotate count specified in CL register.
 */
#define IF_FLAG_V                       (1 <<  3)
#define IF_FB_V_BIT                     1
#define IF_FB_V_MASK                    (1 << IF_FB_V_BIT)

/** Instruction format 'reg' field.
 * reg is the register number.
 */
#define IF_FLAG_REG                     (1 <<  4)
#define IF_FB_REG_BIT                   0
#define IF_FB_REG_MASK                  (0b111 << IF_FB_REG_BIT)
#define IF_SB_REG_BIT                   3
#define IF_SB_REG_MASK                  (0b111 <<  IF_SB_REG_BIT)

/** Instruction format 'sreg' field.
 * sreg is the segment register number.
 */
#define IF_FLAG_SREG                    (1 <<  5)
#define IF_FB_SREG_BIT                  3
#define IF_FB_SREG_MASK                 (0b11 << IF_FB_SREG_BIT)
#define IF_SB_SREG_BIT                   3
#define IF_SB_SREG_MASK                  (0b11 <<  IF_SB_SREG_BIT)

/** Instruction format 'esc' field - called 'xxx'/'yyy' field in 8086's docs.
 * These are a 6-bit user given constant.
 */
#define IF_FLAG_ESC                     (1 <<  6)
#define IF_FB_ESC_BIT                   0
#define IF_FB_ESC_MASK                  (0b111 <<  IF_FB_ESC_BIT)
#define IF_SB_ESC_BIT                   3
#define IF_SB_ESC_MASK                  (0b111 <<  IF_FB_ESC_BIT)


/** Instruction format 'mod r/m' field.
 * These set the addressing mode.
 */
#define IF_FLAG_MOD_RM                  (1 <<  7)
#define IF_SB_MOD_BIT                   6
#define IF_SB_MOD_MASK                  (0b11 <<  IF_SB_MOD_BIT)
#define IF_SB_RM_BIT                    0
#define IF_SB_RM_MASK                   (0b111 <<  IF_SB_RM_BIT)

#define IF_MOD_MEMORY_DISP0             0
#define IF_MOD_MEMORY_DISP8             1
#define IF_MOD_MEMORY_DISP16            2
#define IF_MOD_REGISTER                 3

#define IF_RM_REGISTER_AL               0
#define IF_RM_REGISTER_AX               0
#define IF_RM_REGISTER_CL               1
#define IF_RM_REGISTER_CX               1
#define IF_RM_REGISTER_DL               2
#define IF_RM_REGISTER_DX               2
#define IF_RM_REGISTER_BL               3
#define IF_RM_REGISTER_BX               3
#define IF_RM_REGISTER_AH               4
#define IF_RM_REGISTER_SP               4
#define IF_RM_REGISTER_CH               5
#define IF_RM_REGISTER_BP               5
#define IF_RM_REGISTER_DH               6
#define IF_RM_REGISTER_SI               6
#define IF_RM_REGISTER_BH               7
#define IF_RM_REGISTER_DI               7

#define IF_RM_MEMORY_BX_SI              0
#define IF_RM_MEMORY_BX_SI_DISP8        0
#define IF_RM_MEMORY_BX_SI_DISP16       0
#define IF_RM_MEMORY_BX_DI              1
#define IF_RM_MEMORY_BX_DI_DISP8        1
#define IF_RM_MEMORY_BX_DI_DISP16       1
#define IF_RM_MEMORY_BP_SI              2
#define IF_RM_MEMORY_BP_SI_DISP8        2
#define IF_RM_MEMORY_BP_SI_DISP16       2
#define IF_RM_MEMORY_BP_DI              3
#define IF_RM_MEMORY_BP_DI_DISP8        3
#define IF_RM_MEMORY_BP_DI_DISP16       3
#define IF_RM_MEMORY_SI                 4
#define IF_RM_MEMORY_SI_DISP8           4
#define IF_RM_MEMORY_SI_DISP16          4
#define IF_RM_MEMORY_DI                 5
#define IF_RM_MEMORY_DI_DISP8           5
#define IF_RM_MEMORY_DI_DISP16          5
#define IF_RM_MEMORY_DIRECT_ADDRESS     6
#define IF_RM_MEMORY_BP_DISP8           6
#define IF_RM_MEMORY_BP_DISP16          6
#define IF_RM_MEMORY_BX                 7
#define IF_RM_MEMORY_BX_DISP8           7
#define IF_RM_MEMORY_BX_DISP16          7

/** Instruction format data bytes.
 * Data width is:
 * if 's' and 'w' in FB: s=0: 16-bit, s=1: 8-bit.
 * if only 'w' in FB: w=0: 8-bit, w=1: 16-bit.
 * else: 16-bit
 */
#define IF_FLAG_DATA                    (1 <<  8)
#define IF_FLAG_ADDR16                  (1 <<  9)
#define IF_FLAG_DISP16                  (1 << 10)
#define IF_FLAG_SEG16                   (1 << 10)
#define IF_FLAG_DISP8                   (1 << 11)
#define IF_FLAG_DATA8                   (1 << 12)
#define IF_FLAG_AX_SRC                  (1 << 13)
#define IF_FLAG_AX_DST                  (1 << 14)
#define IF_FLAG_DX                      (1 << 15)

/**
 * @struct if_t
 */
typedef struct{
    uint16_t flags;
    uint8_t first_byte;
    uint8_t second_byte;
} if_t;



// Instructions:

/*
 * prefixes are: LOCK, REP, Segment Override, then instruction.
 * (original 8086 ASM generated them in this order). 
 * LOCK and REP are specified by the user with an insruction. 
 * segment override is part of the operands for an instruction. 
 */

/** Instruction mnemonic.
 * "LOOPNZ" is the longest mnemonic with 6 letters.
 * 
 * Pointer is 8 bytes, so it's better just saving the mnemonic directly.
 * Even on the 8086 itself where a pointer is 2 bytes,
 * it takes about the same memory overall, but saves on
 * additional memory access that is needed with pointers.
 */
#define MAX_MNEMONIC_LENGTH             6

/** @struct instruction_t
 * @param mnemonic Instruction mnemonic.
 * @param format Instruction binary format.
 */
typedef struct{
    const char mnemonic[MAX_MNEMONIC_LENGTH + 1];
    const uint8_t formats_length;
    const if_t *formats;
} instruction_t;

extern instruction_t instructions[];
extern uint16_t instructions_length;

typedef struct{
    const char synonym[MAX_MNEMONIC_LENGTH + 1];
    const char mnemonic[MAX_MNEMONIC_LENGTH + 1];
} instruction_synonym_t;

extern instruction_synonym_t instructions_synonyms[];
extern uint16_t instructions_synonyms_length;

#endif /* ISA_H */