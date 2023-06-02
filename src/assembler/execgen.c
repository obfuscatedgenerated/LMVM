#include "assembler/execgen.h"
#include "assembler/lexer.h"

#include <stdlib.h>
#include <string.h>

// converts tokens into a sequence of unsigned integers (little man's machine binary)
// classic LMC has 100 memory addresses, so the executable is 100 unsigned integers
// we may expand this when extended LMC is implemented
unsigned int *generate_executable(token_ll_node_st *tokens_head) {
    unsigned int *executable = calloc(EXECUTABLE_SIZE, sizeof(unsigned int));

    // convert each token into the machine code
    token_ll_node_st *current = tokens_head;
    size_t index = 0;
    while (current != NULL && index < EXECUTABLE_SIZE - 1) {
        char *mnemonic = current->token->mnemonic;

        // c doesn't have a switch statement for strings, so we have to use if else ladders
        // I <3 C
        // we could do some overengineered hash table, but that seems bloated and pointless
        if (strcmp(mnemonic, "INP") == 0) {
            executable[index] = LMC_INP;
        } else if (strcmp(mnemonic, "OUT") == 0) {
            executable[index] = LMC_OUT;
        } else if (strcmp(mnemonic, "HLT") == 0) {
            executable[index] = LMC_HLT;
        } else {
            // TODO; implement the rest of the mnemonics
        }

        index++;
        current = current->next;
    }

    return executable;
}
