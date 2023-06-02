#ifndef LMVM_VALIDATOR_H
#define LMVM_VALIDATOR_H

#include "lexer.h"

/**
 * Represents a node in a doubly linked list of labels.
 * @see label_doubly_ll_node_st
 */
struct label_doubly_ll_node_s {
    char *label;
    struct label_doubly_ll_node_s *next;
    struct label_doubly_ll_node_s *previous;
};

/**
 * Represents a node in a doubly linked list of labels.
 * @see label_doubly_ll_node_s
 */
typedef struct label_doubly_ll_node_s label_doubly_ll_node_st;


/**
 * Represents a result from the label validator.
 * @see label_validation_result_et
 */
enum label_validation_result_e {
    LABEL_VALIDATION_RESULT_OK_DOESNT_EXIST = 0,
    LABEL_VALIDATION_RESULT_OK_EXISTS = 1,
    LABEL_VALIDATION_RESULT_INVALID = 2,
};

/**
 * Represents a result from the label validator.
 * @see label_validation_result_e
 */
typedef enum label_validation_result_e label_validation_result_et;


/**
 * Runs all validation checks on the given list of tokens.
 *
 * @param tokens  The head of the list of tokens
 * @return        0 if valid, 1 if invalid
 */
int validate(token_ll_node_st *tokens);

#endif //LMVM_VALIDATOR_H
