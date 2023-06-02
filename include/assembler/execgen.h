#ifndef LMVM_EXECGEN_H
#define LMVM_EXECGEN_H

#include "assembler/lexer.h"

/**
 * Converts the given list of tokens into an LMCX executable.
 *
 * @param tokens_head  The head of the list of tokens
 * @return             The values of the executable's cells
 */
unsigned int *generate_executable(token_ll_node_st *tokens_head);

#endif //LMVM_EXECGEN_H
