#ifndef LMVM_PARSER_H
#define LMVM_PARSER_H

#include "lexer.h"
#include "common/hashtable/kv_dict.h"

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
 * Validates given label name.
 *
 * @param label The label name to validate
 * @param known_labels_current Doubly linked list of known labels, or NULL to disable checking for duplicates
 * @return A label_validation_result_et representing the result of the validation
 */
label_validation_result_et validate_label_name(char *label, label_doubly_ll_node_st **known_labels_current);

/**
 * Runs all validation checks on the given list of tokens, and returns a hash table of labels to memory addresses.
 *
 * @param tokens_head  The head of the list of tokens
 * @return             A hash table of labels to memory addresses, or NULL if validation failed
 */
kv_dict *parse_tokens(token_ll_node_st *tokens_head);

#endif //LMVM_PARSER_H
