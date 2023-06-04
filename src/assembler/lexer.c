#include "assembler/lexer.h"
#include "common/executable_props.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <regex.h>
#include <ctype.h>

static regex_t line_whitespace_regex; // global regex

// define mnemonics
static const char *mnemonics[] = {
        "ADD",
        "SUB",
        "STA",
        "LDA",
        "BRA",
        "BRZ",
        "BRP",
        "INP",
        "OUT",
        "HLT",
        "DAT",
};
#define MNEMONIC_COUNT (sizeof(mnemonics) / sizeof(mnemonics[0]))

// lex the line
static tagged_lex_result_st lex_line(char *line, size_t line_idx) {
    // allocate array of tokens (up to 3)
    char *tokens[3];

    // split the line by spaces or tabs
    char *token = strtok(line, "  \t");
    size_t token_idx = 0;

    while (token != NULL) {
        // do not allow more than 3 tokens
        if (token_idx == 3) {
            fprintf(stderr, "\nError: Too many tokens near line %zu. Line content: %s\n", line_idx, line);
            return (tagged_lex_result_st) {
                    .is_status_code = 1,
                    .value = (lex_result_ut) {LEX_STATUS_ERROR}
            };
        }

        // copy the token to the array
        size_t token_len = strlen(token);
        tokens[token_idx] = malloc(token_len + 1);
        memcpy(tokens[token_idx], token, token_len + 1);

        // get the next token
        token = strtok(NULL, "  \t");
        token_idx++;
    }

    // always try to copy the first token to the label
    // if there is no label, this will be overwritten
    size_t label_len = strlen(tokens[0]);
    char *label = malloc(label_len + 1);
    memcpy(label, tokens[0], label_len + 1);

    char *mnemonic = NULL;
    char *operand = NULL;

    size_t operand_idx = 1;

    for (size_t idx = 0; idx < token_idx; idx++) {
        // check for a mnemonic
        if (idx <= operand_idx) {
            // convert token to uppercase (case insensitive mnemonic)
            size_t token_len = strlen(tokens[idx]);
            char *uppercase_token = malloc(token_len + 1);
            memcpy(uppercase_token, tokens[idx], token_len + 1);
            for (size_t i = 0; i < token_len; i++) {
                uppercase_token[i] = (char) toupper(uppercase_token[i]);
            }

            for (size_t mnemonic_idx = 0; mnemonic_idx < MNEMONIC_COUNT; mnemonic_idx++) {
                if (strcmp(uppercase_token, mnemonics[mnemonic_idx]) == 0) {
                    // mnemonic found

                    // if a mnemonic has already been found, put error
                    if (mnemonic != NULL) {
                        fprintf(stderr, "\nError: Duplicate mnemonic encountered near line %zu. Line content: %s\n", line_idx, line);

                        // free token array
                        for (size_t i = 0; i < token_idx; i++) {
                            free(tokens[i]);
                        }

                        return (tagged_lex_result_st) {
                                .is_status_code = 1,
                                .value = (lex_result_ut) {LEX_STATUS_ERROR}
                        };
                    }

                    // if this is the first token, clear and NULL the label variable and decrement the operand index
                    if (idx == 0) {
                        free(label);
                        label = NULL;

                        operand_idx--;

                        // this means that the line is in the format MNE OP rather than lbl MNE OP
                        // therefore, if there are 3 tokens on this line, put error
                        if (token_idx == 3) {
                            fprintf(stderr, "\nError: Too many tokens near line %zu. Line content: %s\n", line_idx, line);

                            // free token array
                            for (size_t i = 0; i < token_idx; i++) {
                                free(tokens[i]);
                            }

                            return (tagged_lex_result_st) {
                                    .is_status_code = 1,
                                    .value = (lex_result_ut) {LEX_STATUS_ERROR}
                            };
                        }
                    }

                    // set the mnemonic
                    mnemonic = malloc(token_len + 1);
                    memcpy(mnemonic, uppercase_token, token_len + 1);
                    break;
                }
            }

            free(uppercase_token);
        } else {
            // if an operand has already been found, put error
            if (operand != NULL) {
                fprintf(stderr, "\nError: Duplicate operand encountered near line %zu. Line content: %s\n", line_idx, line);

                // free token array
                for (size_t i = 0; i < token_idx; i++) {
                    free(tokens[i]);
                }

                return (tagged_lex_result_st) {
                        .is_status_code = 1,
                        .value = (lex_result_ut) {LEX_STATUS_ERROR}
                };
            }

            // set the operand
            size_t token_len = strlen(tokens[idx]);
            operand = malloc(token_len + 1);
            memcpy(operand, tokens[idx], token_len + 1);
        }
    }

    // free token array
    for (size_t i = 0; i < token_idx; i++) {
        free(tokens[i]);
    }

    // if no mnemonic has been found, put error
    if (mnemonic == NULL) {
        fprintf(stderr, "\nError: Missing or invalid mnemonic encountered near line %zu. Line content: %s\n", line_idx, line);
        return (tagged_lex_result_st) {
                .is_status_code = 1,
                .value = (lex_result_ut) {LEX_STATUS_ERROR}
        };
    }

    // create value
    token_st *token_struct = malloc(sizeof(token_st));
    token_struct->label = label;
    token_struct->mnemonic = mnemonic;
    token_struct->operand = operand;

    return (tagged_lex_result_st) {
            .is_status_code = 0,
            .value = (lex_result_ut) {.token = token_struct}
    };
}

// prepare and lex a single line of code
static tagged_lex_result_st prepare_and_lex_line(char *line, size_t line_idx) {
    // execute the regex on the line (assured to be compiled at this point)
    int regex_result = regexec(&line_whitespace_regex, line, 0, NULL, 0);

    // if the line only contains whitespace, return an empty line status code
    if (regex_result != REG_NOMATCH) {
        return (tagged_lex_result_st) {
                .is_status_code = 1,
                .value = (lex_result_ut) {LEX_STATUS_NO_CONTENT}
        };
    }

    // strip leading whitespace from the line
    while (line[0] == ' ' || line[0] == '\t') {
        line++;
    }

    // if no content remains in the line, return an empty line status code
    if (strlen(line) == 0) {
        return (tagged_lex_result_st) {
                .is_status_code = 1,
                .value = (lex_result_ut) {LEX_STATUS_NO_CONTENT}
        };
    }

    // strip comments from the line (all text after a ;, including the ;)
    char *comment_start = strchr(line, ';');
    if (comment_start != NULL) {
        *comment_start = '\0';
    }

    // if no content remains in the line, return an empty line status code
    size_t line_length = strlen(line);
    if (line_length == 0) {
        return (tagged_lex_result_st) {
                .is_status_code = 1,
                .value = (lex_result_ut) {LEX_STATUS_NO_CONTENT}
        };
    }

    // strip trailing whitespace from the line
    while (line[line_length - 1] == ' ' || line[line_length - 1] == '\t') {
        line[line_length - 1] = '\0';
        line_length--;
    }

    // if no content remains in the line, return an empty line status code
    if (strlen(line) == 0) {
        return (tagged_lex_result_st) {
                .is_status_code = 1,
                .value = (lex_result_ut) {LEX_STATUS_NO_CONTENT}
        };
    }

    // if content remains in the line, continue lexing
    return lex_line(line, line_idx);
}


// add to the linked list of tokens
static void push_to_tokens(token_ll_node_st **current, token_st *token) {
    // create a new node
    token_ll_node_st *new_node = malloc(sizeof(token_ll_node_st));
    new_node->token = token;
    new_node->next = NULL;

    // if the current node is NULL, set the current node to the new node
    if (*current == NULL) {
        *current = new_node;
        return;
    }

    // otherwise, iterate through the linked list until the end is reached
    token_ll_node_st *current_node = *current;
    while (current_node->next != NULL) {
        current_node = current_node->next;
    }

    // set the next node to the new node
    current_node->next = new_node;
}


// lex an entire program, returning the head of a linked list of tokens
token_ll_node_st *lex(char *code) {
    // compile a regex to check if the line only contains whitespace (used later)
    int reg_comp_result = regcomp(&line_whitespace_regex, "^[[:space:]]*$", 0);
    if (reg_comp_result != 0) {
        fputs("Internal Error: Could not compile line_whitespace_regex.", stderr);
        exit(1);
    }

    // create a linked list to store the tokens, setting the current node to the head
    token_ll_node_st *head_token = NULL;
    token_ll_node_st **current_token = &head_token;
    unsigned int pushed_tokens = 0;

    // run line-by-line lexing by splitting by newlines
    char *strmax;
    // TODO: implement own scanner/tokeniser that doesn't skip consecutive newlines
    // this would mean that the line_idx would be correct for errors, so no need to use imprecise language
    char *line = strtok_r(code, "\n", &strmax);
    size_t line_idx = 0;

    while (line != NULL) {
        // strip \r from the end of the line (code expects LF, not CRLF)
        if (line[strlen(line) - 1] == '\r') {
            line[strlen(line) - 1] = '\0';
        }

        tagged_lex_result_st res = prepare_and_lex_line(line, line_idx);

        // return error if the line is invalid
        if (res.is_status_code != 0 && res.value.status_code != LEX_STATUS_NO_CONTENT) {
            exit(res.value.status_code);
        }

        // push the token to the linked list
        if (res.value.token != NULL) {
            if (pushed_tokens >= EXECUTABLE_SIZE) {
                fputs("Error: Program is too large to fit in memory.", stderr);
                exit(1);
            }

            push_to_tokens(current_token, res.value.token);
            pushed_tokens++;
        }

        // use strtok_r to keep context across the scope of the loop
        line = strtok_r(NULL, "\n", &strmax);
        line_idx++;
    }

    return head_token;
}
