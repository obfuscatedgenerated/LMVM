#include "assembler/lexer.h"
#include "assembler/validator.h"
#include "assembler/execgen.h"
#include "common/executable_props.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

int main() {
    char *code = "; Count to 5\n"
                 "\n"
                 "loop    LDA count  ; load the value of count into the accumulator\n"
                 "        ADD one\n"
                 "        OUT        ; output the value of the accumulator\n"
                 "        STA count  ; hold the value of the accumulator into count\n"
                 "        SUB five   ; subtract 5 from the accumulator...\n"
                 "        BRZ end    ; ...to therefore stop execution if the count is 5 (ACC=0)\n"
                 "        BRA loop   ; else, branch to loop\n"
                 "end     HLT\n"
                 "\n"
                 "\n"
                 "count   DAT 0\n"
                 "one     DAT 1\n"
                 "five    DAT 5";

    // copy code to a buffer
    char *code_buffer = malloc(strlen(code) + 1);
    strcpy(code_buffer, code);

    // lex and validate the code
    token_ll_node_st *tokens_head = lex(code_buffer);
    if (validate(tokens_head) != 0) {
        return 1;
    }

    // generate the executable
    unsigned int *executable = generate_executable(tokens_head);

    // free the tokens
    token_ll_node_st *current = tokens_head;
    while (current != NULL) {
        token_ll_node_st *next = current->next;
        free(current->token);
        // TODO; may need to free each field of token, but it seems to crash when i do that sometimes
        free(current);
        current = next;
    }

    // print the executable as a test
    puts("Executable:");
    for (size_t i = 0; i < EXECUTABLE_SIZE - 1; i++) {
        printf("%zu: %u\n", i, executable[i]);
    }

    return 0;
}
