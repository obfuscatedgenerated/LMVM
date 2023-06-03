#include "assembler/execgen.h"
#include "assembler/lexer.h"
#include "assembler/parser.h"
#include "common/executable_props.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// convert mnemonic to prefix, excluding DAT and non-operand mnemonics
int mnemonic_to_prefix(char * mnemonic) {
    //  using yet another if else ladder
    //  I <3<3 C
    if (strcmp(mnemonic, "ADD") == 0) {
        return LMC_ADD_PREFIX;
    } else if (strcmp(mnemonic, "SUB") == 0) {
        return LMC_SUB_PREFIX;
    } else if (strcmp(mnemonic, "STA") == 0) {
        return LMC_STA_PREFIX;
    } else if (strcmp(mnemonic, "LDA") == 0) {
        return LMC_LDA_PREFIX;
    } else if (strcmp(mnemonic, "BRA") == 0) {
        return LMC_BRA_PREFIX;
    } else if (strcmp(mnemonic, "BRZ") == 0) {
        return LMC_BRZ_PREFIX;
    } else if (strcmp(mnemonic, "BRP") == 0) {
        return LMC_BRP_PREFIX;
    } else {
        return -1;
    }
}

// converts tokens into a sequence of unsigned integers (LMCX executable)
// classic LMC has 100 memory addresses, so the executable is 100 unsigned integers
// we may expand this when extended LMC is implemented
unsigned int *generate_executable(token_ll_node_st *tokens_head, kv_dict *labels_to_addresses) {
    unsigned int *executable = calloc(EXECUTABLE_SIZE, sizeof(unsigned int));

    // convert each token into the machine code
    token_ll_node_st *current = tokens_head;
    size_t index = 0;
    while (current != NULL && index < EXECUTABLE_SIZE - 1) {
        char *mnemonic = current->token->mnemonic;

        // c doesn't have a switch statement for strings, so we have to use if else ladders
        // I <3 C
        // we could do some overengineered hash table, but that seems bloated and pointless
        // parse all mnemonics that don't have operands
        if (strcmp(mnemonic, "INP") == 0) {
            executable[index] = LMC_INP;
        } else if (strcmp(mnemonic, "OUT") == 0) {
            executable[index] = LMC_OUT;
        } else if (strcmp(mnemonic, "HLT") == 0) {
            executable[index] = LMC_HLT;
        } else {
            char *operand_str = current->token->operand;
            size_t operand_value = 0;

            if (operand_str == NULL) {
                fprintf(stderr, "Internal Error: mnemonic \"%s\" has no operand, but validator claimed it does\n", mnemonic);
                return NULL;
            }

            // if the operand is a label, use it to get the memory address as the operand value
            if (validate_label_name(operand_str, NULL) != LABEL_VALIDATION_RESULT_INVALID) {
                size_t *op_value_ptr = get_item(labels_to_addresses, operand_str, strlen(operand_str) + 1);

                if (op_value_ptr == NULL) {
                    fprintf(stderr,
                            "Internal Error: label \"%s\" not found, but validator claimed it exists (error fetching from dict)\n",
                            operand_str);
                    return NULL;
                }

                // dereference the pointer to get the value
                operand_value = *op_value_ptr;
            } else {
                // otherwise, convert the operand to an integer
                operand_value = strtol(operand_str, NULL, 10);
            }

            // TODO: print below in debug mode only
            //printf("string operand: %s value: %zu\n", operand_str, operand_value);

            // check operand is in range
            if (operand_value > 99) {
                fprintf(stderr, "Internal Error: operand \"%s\" is greater than 99, but validator allowed it\n", operand_str);
                return NULL;
            }

            // if the mnemonic is DAT, simply write the operand value to the executable
            if (strcmp(mnemonic, "DAT") == 0) {
                executable[index] = operand_value;

                index++;
                current = current->next;
                continue;
            }

            // otherwise, convert mnemonic into prefix
            int prefix = mnemonic_to_prefix(mnemonic);

            if (prefix == -1) {
                fprintf(stderr, "Internal Error: mnemonic \"%s\" is invalid, but validator allowed it\n", mnemonic);
                return NULL;
            }

            // write the instruction to the executable
            executable[index] = prefix + operand_value;
        }

        index++;
        current = current->next;
    }

    return executable;
}
