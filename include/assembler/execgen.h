#ifndef LMVM_EXECGEN_H
#define LMVM_EXECGEN_H

#include "assembler/lexer.h"
#include "common/hashtable/kv_dict.h"

/**
 * Converts the given list of tokens and table of labels to addresses into an LMCX executable.
 *
 * @param tokens_head  The head of the list of tokens
 * @param labels_to_addresses  A hash table of labels to memory addresses
 * @return             The values of the executable's cells or NULL if the executable could not be generated
 */
unsigned short int *generate_executable(token_ll_node_st *tokens_head, kv_dict *labels_to_addresses);

#endif //LMVM_EXECGEN_H
