// the assembler doesn't need to have a real parser, so this module just validates the label and operand of each instruction.

#include "assembler/validator.h"
#include "assembler/lexer.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>


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


// validates every label in the tokens exists and is used properly
int validate_labels(token_ll_node_st *tokens_head) {
    // prepare a linked list to store the known labels
    label_doubly_ll_node_st *known_labels_head = NULL;
    label_doubly_ll_node_st **known_labels_current = &known_labels_head;

    size_t line_idx = 1;
    token_ll_node_st *current = tokens_head;

    // iterate through the tokens to verify each label name is valid
    while(current != NULL) {
        // DATs must have a label
        if (strcmp(current->token->mnemonic, "DAT") == 0) {
            if (current->token->label == NULL) {
                fprintf(stderr, "Error: DAT near line %zu must have a label. Line has mnemonic: %s\n", line_idx, current->token->mnemonic);
                return 1;
            }
        }

        // if the token has a label, validate it
        if (current->token->label != NULL) {
            // check the label is valid
            int label_validation_result = validate_label_name(current->token->label, known_labels_current);
            if (label_validation_result != LABEL_VALIDATION_RESULT_OK_DOESNT_EXIST) {
                fprintf(stderr, "Error: label \"%s\" near line %zu is invalid or already exists. Line has mnemonic: %s\n", current->token->label, line_idx, current->token->mnemonic);
                return 1;
            }

            // add the label to the known labels
            push_known_label(current->token->label, known_labels_current);
        }

        line_idx++;
        current = current->next;
    }


    line_idx = 1;
    current = tokens_head;

    // after validating and loading in all the label names, check that any label operands refer to a valid label
    while(current != NULL) {
        // if the token has an operand that consists only of letters, it must be a label
        // check the label exists
        if (current->token->operand != NULL) {
            int operand_validation_result = validate_label_name(current->token->operand, known_labels_current);
            if (operand_validation_result == LABEL_VALIDATION_RESULT_OK_DOESNT_EXIST) {
                fprintf(stderr, "Error: label \"%s\" near line %zu doesn't exist. Line has mnemonic: %s\n", current->token->operand, line_idx, current->token->mnemonic);
                return 1;
            }
        }

        line_idx++;
        current = current->next;
    }

    return 0;
}

int validate(token_ll_node_st *tokens_head) {
    // validate that INP, OUT, and HLT have no operands
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

    return validate_labels(tokens_head);
}