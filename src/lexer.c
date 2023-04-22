#include "lexer.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <regex.h>

regex_t line_whitespace_regex; // global regex

// define mnemonics
char *mnemonics[] = {
        "LDA",
        "STA",
        "ADD",
        "SUB",
        "OUT",
        "HLT",
        "BRZ",
        "BRA",
        "DAT"
};

char *ext_mnemonics[] = {
        "ERR",
        "TXT",
        "SLP",
        "CST",
        "IMP",
};

// lex the line
int lex_line(char *line, size_t line_idx) {
    // allocate array of tokens (up to 3)
    char *tokens[3];

    // split the line by spaces or tabs
    char *token = strtok(line, "  \t");
    size_t token_idx = 0;

    while (token != NULL) {
        // do not allow more than 3 tokens
        if (token_idx == 3) {
            fprintf(stderr, "\nError: Too many tokens on line %llu.\n", line_idx + 1);
            return 1;
        }

        // add the token to the array
        tokens[token_idx] = token;

        // get the next token
        token = strtok(NULL, "  \t");
        token_idx++;
    }

    char *label = tokens[0];
    char *mnemonic = NULL;
    char *operand = NULL;

    size_t operand_idx = 1;

    for (size_t idx = 0; idx < token_idx; idx++) {
        // check for a mnemonic
        if (idx <= operand_idx) {
            for (size_t mnemonic_idx = 0; mnemonic_idx < 9; mnemonic_idx++) {
                if (strcmp(tokens[idx], mnemonics[mnemonic_idx]) == 0) {
                    // mnemonic found

                    // if a mnemonic has already been found, error
                    if (mnemonic != NULL) {
                        fprintf(stderr, "\nError: Duplicate mnemonic encountered on line %llu.\n", line_idx + 1);
                        return 1;
                    }

                    // if this is the first token, NULL the label variable and decrement the operand index
                    if (idx == 0) {
                        label = NULL;

                        operand_idx--;

                        // this means that the line is in the format MNE OP rather than lbl MNE OP
                        // therefore, if there are 3 tokens on this line, error
                        if (token_idx == 3) {
                            fprintf(stderr, "\nError: Too many tokens on line %llu.\n", line_idx + 1);
                            return 1;
                        }
                    }

                    // set the mnemonic
                    mnemonic = tokens[idx];
                    break;
                }
            }
        } else {
            // if no mnemonic has been found, error
            if (mnemonic == NULL) {
                fprintf(stderr, "\nError: No mnemonic encountered on line %llu.\n", line_idx + 1);
                return 1;
            }

            // if an operand has already been found, error
            if (operand != NULL) {
                fprintf(stderr, "\nError: Duplicate operand encountered on line %llu.\n", line_idx + 1);
                return 1;
            }

            // set the operand
            operand = tokens[idx];
        }
    }

    printf("Label: %s | ", label);
    printf("Mnemonic: %s | ", mnemonic);
    printf("Operand: %s\n", operand);

    // TODO: operands not working

    return 0;
}

// prepare to lex a single line of code
int prepare_line(char *line, size_t line_idx) {
    // execute the regex on the line (assured to be compiled at this point)
    int regex_result = regexec(&line_whitespace_regex, line, 0, NULL, 0);

    // if the line only contains whitespace, return
    if (regex_result != REG_NOMATCH) {
        return 0;
    }

    size_t line_length = strlen(line);

    // strip leading whitespace from the line
    while (line[0] == ' ' || line[0] == '\t') {
        line++;
    }

    // if no content remains in the line, return
    if (strlen(line) == 0) {
        return 0;
    }

    // strip comments from the line (all text after a ;, including the ;)
    char *comment_start = strchr(line, ';');
    if (comment_start != NULL) {
        *comment_start = '\0';
    }

    // if no content remains in the line, return
    if (strlen(line) == 0) {
        return 0;
    }

    // strip trailing whitespace from the line
    while (line[line_length - 1] == ' ' || line[line_length - 1] == '\t') {
        line[line_length - 1] = '\0';
        line_length--;
    }

    // if no content remains in the line, return
    if (strlen(line) == 0) {
        return 0;
    }

    // if content remains in the line, continue lexing
    return lex_line(line, line_idx);
}

// lex an entire program
int lex(char *code) {
    // compile a regex to check if the line only contains whitespace (used later)
    int reg_comp_result = regcomp(&line_whitespace_regex, "^[[:space:]]*$", 0);
    if (reg_comp_result != 0) {
        fputs("Fatal Internal Error: Could not compile line_whitespace_regex.", stderr);
        return 1;
    }

    // run line-by-line lexing by splitting by newlines
    char *strmax;
    char *line = strtok_r(code, "\n", &strmax);
    size_t line_idx = 0;

    while (line != NULL) {
        // make a weak copy of the line to prevent affecting strtok
        char *line_copy = malloc(strlen(line) + 1);
        strcpy(line_copy, line);

        int exit_code = prepare_line(line_copy, line_idx);

        if (exit_code != 0) {
            return exit_code;
        }

        // use strtok_r to keep context across the scope of the loop
        line = strtok_r(NULL, "\n", &strmax);
        line_idx++;
    }

    return 0;
}
