#ifndef LMVM_EXECUTION_H
#define LMVM_EXECUTION_H

#include "common/opcodes.h"

/**
 * Represents the result of executing an instruction.
 * @see execution_result_et
 */
enum execution_result_e {
    EXECUTION_INDETERMINATE,
    EXECUTION_ERROR,
    EXECUTION_SUCCESS_ACC_CHANGED,
    EXECUTION_SUCCESS_ACC_UNCHANGED,
    EXECUTION_SUCCESS_BRANCHED,
    EXECUTION_HALT
};

/**
 * Represents the result of executing an instruction.
 * @see execution_result_e
 */
typedef enum execution_result_e execution_result_et;


/**
 * Executes the given instruction.
 *
 * @param opcode  The opcode to execute.
 * @param reg_MAR The memory address register.
 * @param reg_ACC The accumulator register.
 * @param reg_PC The program counter register.
 * @return The result of executing the instruction.
 */
execution_result_et
execute(lmc_opcode_et opcode, unsigned short int *reg_MAR, unsigned short int *reg_ACC, unsigned short int *reg_PC);

#endif //LMVM_EXECUTION_H
