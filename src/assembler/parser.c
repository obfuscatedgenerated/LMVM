// the assembler doesn't need to have a real parser, so this module just validates the label and operand of each instruction.

#include "assembler/parser.h"
#include "assembler/lexer.h"
#include "common/hashtable/kv_dict.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>


// parse_tokens that INP, OUT, and HLT have no operands
int validate_inp_out_hlt(token_ll_node_st *tokens_head) {
    token_ll_node_st *current = tokens_head;
    size_t line_idx = 1;

    while(current != NULL) {
        if (strcmp(current->token->mnemonic, "INP") == 0 || strcmp(current->token->mnemonic, "OUT") == 0 || strcmp(current->token->mnemonic, "HLT") == 0) {
            if (current->token->operand != NULL) {
                fprintf(stderr, "Error: mnemonic \"%s\" near line %zu must not have an operand.\n", current->token->mnemonic, line_idx);
                return 1;
            }
        }

        line_idx++;
        current = current->next;
    }

    return 0;
}


// checks it consists only of letters, and hasn't been used before
// returns 0 if valid, 1 if invalid, 2 if already used
label_validation_result_et validate_label_name(char *label, label_doubly_ll_node_st **known_labels_current) {
    // check the label consists only of letters
    char *current_char = label;
    while (*current_char != '\0') {
        if ((*current_char < 'A' || *current_char > 'Z') && (*current_char < 'a' || *current_char > 'z')) {
            return LABEL_VALIDATION_RESULT_INVALID;
        }
        current_char++;
    }

    if (known_labels_current == NULL) {
        return LABEL_VALIDATION_RESULT_OK_DOESNT_EXIST;
    }

    // search backwards through the known labels to check the label hasn't been used before
    // don't affect the value of known_labels_current for the user of the function
    label_doubly_ll_node_st *known_labels_current_copy = *known_labels_current;
    while (known_labels_current_copy != NULL) {
        if (strcmp(known_labels_current_copy->label, label) == 0) {
            return LABEL_VALIDATION_RESULT_OK_EXISTS;
        }
        known_labels_current_copy = known_labels_current_copy->previous;
    }

    return LABEL_VALIDATION_RESULT_OK_DOESNT_EXIST;
}

void push_known_label(char *label, label_doubly_ll_node_st **known_labels_current) {
    // allocate memory for the new node
    label_doubly_ll_node_st *new_node = malloc(sizeof(label_doubly_ll_node_st));
    new_node->label = label;
    new_node->next = NULL;
    new_node->previous = *known_labels_current;

    // update the current node
    *known_labels_current = new_node;
}


// validates every label in the tokens exists and is used properly, as well as building and returning a hash table of label to memory address
kv_dict *parse_labels(token_ll_node_st *tokens_head) {
    // prepare a linked list to store the known labels
    label_doubly_ll_node_st *known_labels_head = NULL;
    label_doubly_ll_node_st **known_labels_current = &known_labels_head;

    // prepare the hash table to store the label to memory address mapping
    kv_dict *label_to_address_dict = new_dict();

    size_t mem_idx = 0;
    token_ll_node_st *current = tokens_head;

    // iterate through the tokens to verify each label name is valid
    while(current != NULL) {
        // DATs must have a label
        if (strcmp(current->token->mnemonic, "DAT") == 0) {
            if (current->token->label == NULL) {
                fprintf(stderr, "Error: DAT on instruction %zu must have a label. Line has mnemonic: %s\n", mem_idx + 1, current->token->mnemonic);
                return NULL;
            }
        }

        // if the token has a label, validate it
        if (current->token->label != NULL) {
            // check the label is valid
            int label_validation_result = validate_label_name(current->token->label, known_labels_current);
            if (label_validation_result != LABEL_VALIDATION_RESULT_OK_DOESNT_EXIST) {
                fprintf(stderr, "Error: label \"%s\" on instruction %zu is invalid or already exists. Line has mnemonic: %s\n", current->token->label, mem_idx + 1, current->token->mnemonic);
                return NULL;
            }

            // add the label to the known labels
            push_known_label(current->token->label, known_labels_current);

            // push the label and memory address (of the instruction) to the hash table
            size_t *heap_mem_idx = malloc(sizeof(size_t));
            *heap_mem_idx = mem_idx;
            set_item(label_to_address_dict, current->token->label, strlen(current->token->label) + 1, heap_mem_idx);
            // TODO: print below in debug mode only
            //printf("Added label %s to address %zu\n", current->token->label, mem_idx);
        }

        mem_idx++;
        current = current->next;
    }


    mem_idx = 0;
    current = tokens_head;

    // after validating and loading in all the label names, check that any label operands refer to a valid label
    while(current != NULL) {
        // if the token has an operand that consists only of letters, it must be a label
        // check the label exists
        if (current->token->operand != NULL) {
            int operand_validation_result = validate_label_name(current->token->operand, known_labels_current);
            if (operand_validation_result == LABEL_VALIDATION_RESULT_OK_DOESNT_EXIST) {
                fprintf(stderr, "Error: label \"%s\" on instruction %zu doesn't exist. Line has mnemonic: %s\n", current->token->operand, mem_idx + 1, current->token->mnemonic);
                return NULL;
            }
        }

        mem_idx++;
        current = current->next;
    }

    return label_to_address_dict;
}

// all numerical operands must be between 0 and 99
int validate_numerical_operands(token_ll_node_st *tokens_head) {
    token_ll_node_st *current = tokens_head;
    size_t line_idx = 1;

    while (current != NULL) {
        // if the operand is NULL, skip
        if (current->token->operand == NULL) {
            current = current->next;
            line_idx++;
            continue;
        }

        // if the operand is a label, skip
        if (validate_label_name(current->token->operand, NULL) != LABEL_VALIDATION_RESULT_INVALID) {
            current = current->next;
            line_idx++;
            continue;
        }

        // if the operand is not numerical, it is a syntax error
        for (size_t i = 0; i < strlen(current->token->operand); i++) {
            if (current->token->operand[i] < '0' || current->token->operand[i] > '9') {
                fprintf(stderr, "Error: operand \"%s\" near line %zu is not numerical or a valid label. Line has mnemonic: %s\n", current->token->operand, line_idx, current->token->mnemonic);
                return 1;
            }
        }

        // if the operand is numerical, check it is between 0 and 99
        int value = strtol(current->token->operand, NULL, 10);
        if (value < 0 || value > 99) {
            fprintf(stderr, "Error: operand \"%s\" near line %zu is not between 0 and 99. Line has mnemonic: %s\n", current->token->operand, line_idx, current->token->mnemonic);
            return 1;
        }

        line_idx++;
        current = current->next;
    }

    return 0;
}


kv_dict *parse_tokens(token_ll_node_st *tokens_head) {
    if (validate_inp_out_hlt(tokens_head) != 0) {
        return NULL;
    }

    kv_dict *label_to_address_dict = parse_labels(tokens_head);
    if (label_to_address_dict == NULL) {
        return NULL;
    }

    if (validate_numerical_operands(tokens_head) != 0) {
        return NULL;
    }

    return label_to_address_dict;
}
