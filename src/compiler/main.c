#include "compiler/lexer.h"

#include <string.h>
#include <stdlib.h>

int main() {
    char *code = "; Count to 5\n"
                 "\n"
                 "loop    LDA count  ; load the value of count into the accumulator\n"
                 "        ADD one\n"
                 "        OUT        ; output the value of the accumulator\n"
                 "        STA count  ; hold the value of the accumulator into count\n"
                 "        SUB five     ; subtract 5 from the accumulator...\n"
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

    return lex(code_buffer);
}
