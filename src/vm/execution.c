#include "vm/execution.h"
#include "common/opcodes.h"

#include <stdio.h>
#include <limits.h>
#include <stdlib.h>
#include <errno.h>


static void op_add(
        unsigned short int mdr,
        int *reg_ACC,
        unsigned short int *reg_PC,
        execution_result_et *result
) {
    // check value will stay within range of int
    if (*reg_ACC + mdr > INT_MAX) {
        fprintf(stderr, "Error: Accumulator overflow: %u + %u > %u\n", *reg_ACC, mdr, INT_MAX);
        *result = EXECUTION_ERROR;
        return;
    }

    // doesn't need to stay within 0-999, this will be checked when STA is executed

    *reg_ACC += mdr;
    *reg_PC += 1;

    *result = EXECUTION_SUCCESS_ACC_CHANGED;
}

static void op_sub(
        unsigned short int mdr,
        int *reg_ACC,
        unsigned short int *reg_PC,
        execution_result_et *result
) {
    // check value will stay within range of int
    if (*reg_ACC - mdr < INT_MIN) {
        fprintf(stderr, "Error: Accumulator underflow: %u - %u < %u\n", *reg_ACC, mdr, INT_MIN);
        *result = EXECUTION_ERROR;
        return;
    }

    // doesn't need to stay within 0-999, this will be checked when STA is executed

    *reg_ACC -= mdr;
    *reg_PC += 1;

    *result = EXECUTION_SUCCESS_ACC_CHANGED;
}


static void op_sta(
        const unsigned short int *reg_MAR,
        const int *reg_ACC,
        unsigned short int *reg_PC,
        execution_result_et *result,
        unsigned short int *memory
) {
    // check value is within range of memory
    if (*reg_ACC < 0 || *reg_ACC > 999) {
        fprintf(stderr, "Error: Accumulator value out of memory range: %d\n", *reg_ACC);
        *result = EXECUTION_ERROR;
        return;
    }

    memory[*reg_MAR] = *reg_ACC;
    *reg_PC += 1;

    *result = EXECUTION_SUCCESS_ACC_UNCHANGED;
}

static void op_lda(
        const unsigned short int *reg_MAR,
        int *reg_ACC,
        unsigned short int *reg_PC,
        execution_result_et *result,
        const unsigned short int *memory
) {
    *reg_ACC = memory[*reg_MAR];
    *reg_PC += 1;

    *result = EXECUTION_SUCCESS_ACC_CHANGED;
}


static void op_bra(
        const unsigned short int *reg_MAR,
        unsigned short int *reg_PC,
        execution_result_et *result
) {
    *reg_PC = *reg_MAR;

    *result = EXECUTION_SUCCESS_BRANCHED;
}

static void op_brz(
        const unsigned short int *reg_MAR,
        const int *reg_ACC,
        unsigned short int *reg_PC,
        execution_result_et *result
) {
    if (*reg_ACC == 0) {
        op_bra(reg_MAR, reg_PC, result);
    } else {
        *reg_PC += 1;
        *result = EXECUTION_SUCCESS_ACC_UNCHANGED;
    }
}

static void op_brp(
        const unsigned short int *reg_MAR,
        const int *reg_ACC,
        unsigned short int *reg_PC,
        execution_result_et *result
) {
    if (*reg_ACC >= 0) {
        op_bra(reg_MAR, reg_PC, result);
    } else {
        *reg_PC += 1;
        *result = EXECUTION_SUCCESS_ACC_UNCHANGED;
    }
}


static void op_inp(
        int *reg_ACC,
        unsigned short int *reg_PC,
        execution_result_et *result
) {
    // TODO: this op will accept the range of ACC, but this wont be guaranteed to be within 0-999
    // should we limit either this op or the range of ACC to 0-999, or just leave it?
    // for now, we'll just leave it

    // get number of digits to accept
    static int accept_digits = -1;

    if (accept_digits == -1) {
        int max_digits = 0;
        int min_digits = 0;

        int max = INT_MAX;
        int min = INT_MIN;

        while (max != 0) {
            max /= 10;
            max_digits++;
        }

        while (min != 0) {
            min /= 10;
            min_digits++;
        }

        // accept the number of digits that is larger
        if (min_digits > max_digits) {
            accept_digits = min_digits;
        } else {
            accept_digits = max_digits;
        }
    }

    // read user input up to the number of digits
    int input;
    while (1) {
        char in_buf[accept_digits + 1];

        if (fgets(in_buf, accept_digits + 1, stdin) == NULL && errno != 0) {
            fprintf(stderr, "Invalid input: failed to read\n");
            fprintf(stderr, "Errno: %d\n", errno);

            continue;
        }

        // check input is a valid number
        char *end_ptr;
        input = strtol(in_buf, &end_ptr, 10);

        if (end_ptr == in_buf) {
            puts("Invalid input: not a number\n");
            continue;
        }

        // TODO: check input is within range of int. not sure how we can, have to deal with wrapping around for now

        // input is valid, break out of loop
        break;
    }

    *reg_ACC = input;
    *reg_PC += 1;

    *result = EXECUTION_SUCCESS_ACC_CHANGED;
}

static void op_out(
        const int *reg_ACC,
        unsigned short int *reg_PC,
        execution_result_et *result
) {
    printf("%d\n", *reg_ACC);
    *reg_PC += 1;

    *result = EXECUTION_SUCCESS_ACC_UNCHANGED;
}

// the halt op is written inline in the execute function


execution_result_et execute(
        lmc_opcode_et opcode,
        unsigned short int *reg_MAR,
        int *reg_ACC,
        unsigned short int *reg_PC,
        unsigned short int *memory
) {
    unsigned short int mdr = memory[*reg_MAR]; // memory data register
    execution_result_et result = EXECUTION_INDETERMINATE;

    switch (opcode) {
        case OP_LMC_ADD:
            op_add(mdr, reg_ACC, reg_PC, &result);
            break;
        case OP_LMC_SUB:
            op_sub(mdr, reg_ACC, reg_PC, &result);
            break;
        case OP_LMC_STA:
            op_sta(reg_MAR, reg_ACC, reg_PC, &result, memory);
            break;
        case OP_LMC_LDA:
            op_lda(reg_MAR, reg_ACC, reg_PC, &result, memory);
            break;
        case OP_LMC_BRA:
            op_bra(reg_MAR, reg_PC, &result);
            break;
        case OP_LMC_BRZ:
            op_brz(reg_MAR, reg_ACC, reg_PC, &result);
            break;
        case OP_LMC_BRP:
            op_brp(reg_MAR, reg_ACC, reg_PC, &result);
            break;
        case OP_LMC_IO_OP_INP:
            op_inp(reg_ACC, reg_PC, &result);
            break;
        case OP_LMC_IO_OP_OUT:
            op_out(reg_ACC, reg_PC, &result);
            break;
        case OP_LMC_HLT:
            result = EXECUTION_HALT;
            break;
        default:
            fprintf(stderr, "Error: Invalid opcode: %d\n", opcode);
            result = EXECUTION_ERROR;
            break;
    }

    return result;
}
