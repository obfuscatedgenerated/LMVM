#ifndef LMVM_OPCODES_H
#define LMVM_OPCODES_H

/**
 * Represents the opcodes for the LMC.
 * @see lmc_opcode_et
 */
enum lmc_opcode_e {
    OP_LMC_ADD = 1,
    OP_LMC_SUB = 2,
    OP_LMC_STA = 3,
    // 4xx is unused
    OP_LMC_LDA = 5,
    OP_LMC_BRA = 6,
    OP_LMC_BRZ = 7,
    OP_LMC_BRP = 8,
    OP_LMC_IO_OP = 9
};

/**
 * Represents the opcodes for the LMC.
 * @see lmc_opcode_e
 */
typedef enum lmc_opcode_e lmc_opcode_et;

#define LMC_IO_OP_INP 901
#define LMC_IO_OP_OUT 902
#define LMC_IO_OP_HLT 000

#endif //LMVM_OPCODES_H
