#include "assembler/execgen.h"
#include "assembler/lexer.h"

#include <stdlib.h>

// converts tokens into a sequence of unsigned integers (little man's machine binary)
// classic LMC has 100 memory addresses, so the executable is 100 unsigned integers
// we may expand this when extended LMC is implemented
unsigned int *generate_executable(token_ll_node_st *tokens_head) {
    return calloc(100, sizeof(unsigned int));
}
