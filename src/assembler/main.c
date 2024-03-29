#include "assembler/lexer.h"
#include "assembler/parser.h"
#include "assembler/execgen.h"
#include "common/executable_props.h"
#include "common/file_io.h"
#include "common/checked_alloc.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>
#include <libgen.h>
#include <unistd.h>

#ifndef VERSION_MAJOR
#define VERSION_MAJOR 0
#endif
#ifndef VERSION_MINOR
#define VERSION_MINOR 0
#endif
#ifndef VERSION_PATCH
#define VERSION_PATCH 0
#endif
static const unsigned short int VERSION[3] = {VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH};

#define VERSION_STRING "\nLMASM v%u.%u.%u (supporting lmvm-ext %u)\nA component of the Little Man Virtual Machine.\nCopyright (c) 2023 obfuscatedgenerated\nMIT License\n\n"

static int debug_mode;
static int no_overwrite_mode;

static char *infile_path = NULL;
static char *outfile_path = NULL;

static const char *NULL_DEVICE =
#ifdef _WIN32
        "NUL";
#else
"/dev/null";
#endif

static FILE *debugout = NULL;

#define USAGE_STRING "%s [-h | --help] INFILE [-o | --output OUTFILE] [optional-flags]\n"
#define OPTIONS "-ho:kvdx"
static const struct option LONG_OPTIONS[] = {
        {"help",         no_argument,       NULL,         'h'},
        {"output",       required_argument, NULL,         'o'},
        {"no-overwrite", no_argument, &no_overwrite_mode, 'k'},
        {"version",      no_argument,       NULL,         'v'},
        {"debug",        no_argument, &debug_mode,        'd'},
        {"silent",       no_argument,       NULL,         'x'},
        {NULL,           0,                 NULL,         0}
};


static void parse_args(int argc, char **argv) {
    int c;
    while ((c = getopt_long(argc, argv, OPTIONS, LONG_OPTIONS, NULL)) != -1) {
        switch (c) {
            case 'h':
                puts("\nUsage:");
                printf(USAGE_STRING, argv[0]);
                puts("\n-h | --help:               Show this help message and exit");
                puts("\nRequired positional arguments:");
                puts("INFILE:                    The input entrypoint to assemble");
                puts("\nOptional arguments:");
                puts("-o | --output OUTFILE:     The output file to write the executable to. Defaults to the same file name (with executable extension) in the current directory");
                puts("-k | --no-overwrite:       Keep the output file if it already exists. Refuses to overwrite.");
                puts("-v | --version:            Show the version number and license information");
                puts("-d | --debug:              Enable debug mode");
                puts("-x | --silent:             Silent mode. No output is printed to stdout or stderr");
                puts("");
                exit(0);
            case 'o':
                outfile_path = optarg;
                break;
            case 'v':
                printf(VERSION_STRING, VERSION[0], VERSION[1], VERSION[2], EXT_SUPPORTED_VERSION);
                break;
            case 'x':
                // redirect stdout and stderr to nowhere, checking success
                if (freopen(NULL_DEVICE, "w", stdout) == NULL || freopen(NULL_DEVICE, "w", stderr) == NULL) {
                    fprintf(stderr, "Error: Failed to redirect stdout and stderr to null device\n");
                    exit(1);
                }
                break;
            case 1:
                infile_path = optarg;
                break;
            case 'd':
                // flag not set if using short form
                debug_mode = 1;
                break;
            case 'k':
                // flag not set if using short form
                no_overwrite_mode = 1;
                break;
            case '?':
            case ':':
                printf(USAGE_STRING, argv[0]);
                exit(1);
        }
    }

    // create custom stream around stdout for debug, going to null if debug mode is disabled
    if (debug_mode) {
        debugout = stdout;
    } else {
        debugout = fopen(NULL_DEVICE, "w");
    }
}

static void check_output_path(void) {
    // check if the output file is a directory
    if (is_dir(outfile_path)) {
        fprintf(stderr, "Error: Output file '%s' is a directory\n", outfile_path);
        exit(1);
    }

    // check if the file already exists
    if (no_overwrite_mode && file_exists_and_accessible(outfile_path)) {
        fprintf(stderr, "Error: Output file '%s' already exists and no-overwrite mode is enabled\n", outfile_path);
        exit(1);
    }
}

static void resolve_output_path(void) {
    // validate the output file if specified
    if (outfile_path != NULL) {
        check_output_path();

        return;
    }

    // if no output file specified, calculate the output file path

    // get the file name of the input file
    char *infile_name = basename(infile_path);

    // copy the file name to a new buffer
    size_t infile_name_len = strlen(infile_name);
    char *outfile_name = checked_malloc(infile_name_len + 1);
    memcpy(outfile_name, infile_name, infile_name_len + 1);

    // replace the extension with .lmc
    char *dot = strrchr(outfile_name, '.');
    size_t default_execfile_ext_len = strlen(DEFAULT_EXECFILE_EXT);
    if (dot == NULL) {
        // no extension, append .lmc
        strncat(outfile_name, DEFAULT_EXECFILE_EXT, default_execfile_ext_len + 1);
    } else {
        // replace the extension with .lmc
        memcpy(dot, DEFAULT_EXECFILE_EXT, default_execfile_ext_len + 1);
    }

    // get the current working directory and create a buffer to fit it and the file name
    char *cwd = getcwd(NULL, 0);
    size_t cwd_len = strlen(cwd);

    outfile_path = checked_malloc(cwd_len + strlen(outfile_name) + 2);
    memcpy(outfile_path, cwd, cwd_len + 1);

    // add a slash if needed, use backslash on windows
    char slash = '/';
#ifdef _WIN32
    slash = '\\';
#endif
    if (outfile_path[cwd_len - 1] != slash) {
        outfile_path[cwd_len] = slash;
        cwd_len++;
    }

    // copy the file name to the end of the cwd
    memcpy(outfile_path + cwd_len, outfile_name, strlen(outfile_name) + 1);

    checked_free(cwd);
    checked_free(outfile_name);
    //checked_free(infile_name);

    check_output_path();
}

// TODO: give debugout to other modules

int main(int argc, char **argv) {
    parse_args(argc, argv);

    // check for input file
    fputs("DEBUG: Input file check\n", debugout);
    if (infile_path == NULL) {
        fputs("Error: No input file specified\n", stderr);
        fprintf(stderr, "\nUsage: ");
        fprintf(stderr, USAGE_STRING, argv[0]);
        exit(1);
    }

    // check if the input file is a directory
    if (is_dir(infile_path)) {
        fprintf(stderr, "Error: Input file '%s' is a directory\n", infile_path);
        exit(1);
    }

    // check input file exists
    if (!file_exists_and_accessible(infile_path)) {
        fprintf(stderr, "Error: Input file '%s' does not exist or cannot be opened\n", infile_path);
        exit(1);
    }

    fputs("DEBUG: Resolve output path\n", debugout);
    resolve_output_path();

    printf("Input file: %s\n", infile_path);
    printf("Output file: %s\n", outfile_path);
    puts("Preparing to assemble...");

    // read the file into the code buffer
    fputs("DEBUG: Read input file\n", debugout);
    char *code_buffer = read_text_file(infile_path);

    if (code_buffer == NULL) {
        fprintf(stderr, "Error: Failed to read input file '%s'\n", infile_path);
        exit(1);
    }

    puts("Assembling...");

    // lex and parse the code
    fputs("DEBUG: Lex tokens\n", debugout);
    token_ll_node_st *tokens_head = lex(code_buffer);
    if (tokens_head == NULL) {
        return 1;
    }

    fputs("DEBUG: Parse tokens\n", debugout);
    kv_dict *labels_to_addresses = parse_tokens(tokens_head);
    if (labels_to_addresses == NULL) {
        return 1;
    }

    fputs("DEBUG: Free code buffer\n", debugout);
    checked_free(code_buffer);

    // generate the executable
    fputs("DEBUG: Generate executable\n", debugout);
    unsigned short int *executable = generate_executable(tokens_head, labels_to_addresses);

    // free the tokens
    fputs("DEBUG: Free tokens\n", debugout);
    token_ll_node_st *current = tokens_head;
    while (current != NULL) {
        token_ll_node_st *next = current->next;

        silent_checked_free(current->token->mnemonic);
        silent_checked_free(current->token->operand);
        silent_checked_free(current->token->label);

        checked_free(current->token);
        checked_free(current);
        current = next;
    }

    // free the labels
    fputs("DEBUG: Free labels\n", debugout);
    free_dict(labels_to_addresses);

    fputs("DEBUG: Check executable was built\n", debugout);
    if (executable == NULL) {
        return 1;
    }

    // construct lmcx descriptor
    fputs("DEBUG: Construct LMCX descriptor\n", debugout);
    lmcx_file_descriptor_st *descriptor = checked_malloc(sizeof(lmcx_file_descriptor_st));
    descriptor->data = executable;

    // TODO: trim executable size (remove trailing 0s) (ISSUE #2)
    // for this ^^ we'd need to check what addresses are used. we can't eliminate 0s that are used by DATs (might be a cell simply set to 0)
    descriptor->data_size = EXECUTABLE_SIZE;

    // don't enable extended features
    descriptor->ext_version = 0;
    // to enable: descriptor->ext_version = EXT_SUPPORTED_VERSION;

    // save the executable
    fputs("DEBUG: Write executable to file\n", debugout);
    write_lmcx_file(descriptor, outfile_path, 1);

    fputs("DEBUG: Free executable\n", debugout);
    checked_free(executable);
    fputs("DEBUG: Free descriptor\n", debugout);
    checked_free(descriptor);

    puts("Successfully assembled executable.");

    return 0;
}
