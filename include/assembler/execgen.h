#ifndef LMVM_EXECGEN_H
#define LMVM_EXECGEN_H

#include "assembler/lexer.h"

unsigned int *generate_executable(token_ll_node_st *tokens_head);

#define EXECUTABLE_SIZE 100

#define LMC_INP 901
#define LMC_OUT 902
#define LMC_HLT 000

#endif //LMVM_EXECGEN_H
