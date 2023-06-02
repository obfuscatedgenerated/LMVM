#ifndef LMVM_LEXER_H
#define LMVM_LEXER_H

/**
 * Represents a token.
 * @see token_st
 */
struct token_s {
    char *label;
    char *mnemonic;
    char *operand;
};

/**
 * Represents a token.
 * @see token_s
 */
typedef struct token_s token_st;


/**
 * Represents a result from the lexer, either a status code or a token.
 * @see lex_result_ut
 * @see tagged_lex_result_st
 */
union lex_result_u {
    int status_code;
    token_st *token;
};

/**
 * Represents a result from the lexer, either a status code or a token.
 * @see lex_result_u
 * @see tagged_lex_result_st
 */
typedef union lex_result_u lex_result_ut;

#define LEX_STATUS_NO_CONTENT 0
#define LEX_STATUS_ERROR 1


/**
 * Represents a result from the lexer with a marker to indicate whether it is a status code or a token.
 * @see tagged_lex_result_st
 * @see lex_result_ut
 */
struct tagged_lex_result_s {
    int is_status_code;
    lex_result_ut value;
};

/**
 * Represents a lex result with a marker to indicate whether it is a status code or a token.
 * @see tagged_lex_result_st
 * @see lex_result_ut
 */
typedef struct tagged_lex_result_s tagged_lex_result_st;


/**
 * Represents a node in a linked list of tokens.
 * @see token_ll_node_st
 */
struct token_ll_node_s {
    token_st *token;
    struct token_ll_node_s *next;
};

/**
 * Represents a node in a linked list of tokens.
 * @see token_ll_node_s
 */
typedef struct token_ll_node_s token_ll_node_st;


/**
 * Tokenises the given code into a linked list of tokens.
 *
 * @param code  The code to tokenise
 * @return      A linked list of tokens
 */
token_ll_node_st *lex(char *code);

#endif //LMVM_LEXER_H
