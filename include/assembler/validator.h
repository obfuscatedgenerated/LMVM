#ifndef LMVM_VALIDATOR_H
#define LMVM_VALIDATOR_H

#include "lexer.h"

struct label_doubly_ll_node_s {
    char *label;
    struct label_doubly_ll_node_s *next;
    struct label_doubly_ll_node_s *previous;
};

typedef struct label_doubly_ll_node_s label_doubly_ll_node_st;


enum label_validation_result_e {
    LABEL_VALIDATION_RESULT_OK_DOESNT_EXIST = 0,
    LABEL_VALIDATION_RESULT_OK_EXISTS = 1,
    LABEL_VALIDATION_RESULT_INVALID = 2,
};

typedef enum label_validation_result_e label_validation_result_et;


int validate(token_ll_node_st *tokens);

#endif //LMVM_VALIDATOR_H
