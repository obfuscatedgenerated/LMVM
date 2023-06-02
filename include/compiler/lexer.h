#ifndef LMVM_LEXER_H
#define LMVM_LEXER_H

struct token_s {
    char *label;
    char *mnemonic;
    char *operand;
};

typedef struct token_s token_st;


union lex_result_u {
    int status_code;
    token_st *token;
};

typedef union lex_result_u lex_result_ut;

struct tagged_lex_result_s {
    int is_status_code;
    lex_result_ut value;
};

typedef struct tagged_lex_result_s tagged_lex_result_st;


struct token_ll_node_s {
    token_st *token;
    struct token_ll_node_s *next;
};

typedef struct token_ll_node_s token_ll_node_st;


token_ll_node_st *lex(char *code);

#endif //LMVM_LEXER_H
