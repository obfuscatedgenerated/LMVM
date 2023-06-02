#include "assembler/lexer.h"
#include "assembler/validator.h"
#include "assembler/execgen.h"
#include "common/executable_props.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>

const unsigned int VERSION[3] = {0, 0, 1};

#define VERSION_STRING "\nLMASM v%u.%u.%u\nA component of the Little Man Virtual Machine.\nCopyright 2023 obfuscatedgenerated\nMIT License\n\n"

static int debug_mode = 0;
static int strict_mode = 0;

#define USAGE_STRING "%s [-h | --help] <-o | --output OUTFILE> INFILE [optional-args]\n"
#define OPTIONS "-ho:vDS"
const struct option LONG_OPTIONS[] = {
        {"help",    no_argument,       NULL, 'h'},
        {"output",  required_argument, NULL, 'o'},
        {"version", no_argument,       NULL, 'v'},
        {"debug",   no_argument,       &debug_mode, 'D'},
        {"strict", no_argument,       &strict_mode, 'S'},
        {NULL,      0,                 NULL, 0}
};


void parse_args(int argc, char **argv) {
    int c;
    while ((c = getopt_long(argc, argv, OPTIONS, LONG_OPTIONS, NULL)) != -1) {
        switch (c) {
            case 'h':
                puts("\nUsage:");
                printf(USAGE_STRING, argv[0]);
                puts("\n-h | --help:    Show this help message and exit");
                puts("\nRequired positional arguments:");
                puts("INFILE:           The input entrypoint to assemble");
                puts("\nRequired arguments:");
                puts("-o | --output OUTFILE:  The output file to write the executable to");
                puts("\nOptional arguments:");
                puts("-v | --version:  Show the version number and license information");
                puts("-D | --debug:    Enable debug mode");
                puts("-S | --strict:   Enable strict mode. Warnings are treated as errors");
                puts("");
                exit(0);
            case 'o':
                // TODO
                puts("Output file not yet implemented");
                break;
            case 'v':
                printf(VERSION_STRING, VERSION[0], VERSION[1], VERSION[2]);
            case 1:
                // TODO input file
                puts("Input file not yet implemented");
                break;
            default:
                printf(USAGE_STRING, argv[0]);
                exit(1);
        }
    }
}


int main(int argc, char **argv) {
    parse_args(argc, argv);

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
