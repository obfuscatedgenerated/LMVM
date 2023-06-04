#include "vm/execution.h"
#include "common/opcodes.h"

#include <stdio.h>

execution_result_et execute(
        lmc_opcode_et opcode,
        unsigned short int *reg_MAR,
        unsigned short int *reg_ACC,
        unsigned short int *reg_PC
) {
    // TODO: execute appropriate instruction
    printf("%u %u %u %u\n", opcode, *reg_MAR, *reg_ACC, *reg_PC);
    return EXECUTION_ERROR;
}
