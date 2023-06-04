#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>
#include "common/file_io.h"
#include "common/executable_props.h"

// TODO: consider moving some parsing to common
#define MAJOR_VERSION 0
#define MINOR_VERSION 0
#define PATCH_VERSION 1
static const unsigned short int VERSION[3] = {MAJOR_VERSION, MINOR_VERSION, PATCH_VERSION};

#define VERSION_STRING "\nLMVM v%u.%u.%u (supporting lmvm-ext %u)\nA component of the Little Man Virtual Machine.\nCopyright 2023 obfuscatedgenerated\nMIT License\n\n"

static int debug_mode;

static char *infile_path = NULL;

static const char *NULL_DEVICE =
#ifdef _WIN32
        "NUL";
#else
"/dev/null";
#endif

static FILE *debugout = NULL;

#define USAGE_STRING "%s [-h | --help] INFILE [optional-flags]\n"
#define OPTIONS "-hvdsx"
static const struct option LONG_OPTIONS[] = {
        {"help",         no_argument,       NULL,         'h'},
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
                puts("-v | --version:            Show the version number and license information");
                puts("-d | --debug:              Enable debug mode");
                puts("-x | --silent:             Silent mode. No output is printed to stdout or stderr");
                puts("");
                exit(0);
            case 'v':
                printf(VERSION_STRING, VERSION[0], VERSION[1], VERSION[2], EXT_SUPPORTED_VERSION);
                break;
            case 'x':
                // redirect stdout and stderr to nowhere
                freopen(NULL_DEVICE, "w", stdout);
                freopen(NULL_DEVICE, "w", stderr);
                break;
            case 1:
                infile_path = optarg;
                break;
            case 'd':
                // flag not set if using short form
                debug_mode = 1;
                break;
            case '?': case ':': default:
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

// for now, the VM is just going to interpret the bytecode
// we might add a JIT compiler later (or direct translation to native asm/machine code), but that's a little overengineered for now
// jvm hotspot interprets and then switches to JIT if a method is called a lot

// SPECIAL REGISTERS
//static unsigned short int reg_PC = 0; // program counter
//static unsigned short int reg_ACC = 0; // accumulator
//static unsigned short int reg_CIR = 0; // current instruction register
//static unsigned short int reg_MAR = 0; // memory address register
//static unsigned short int reg_MDR = 0; // memory data register


int main(int argc, char **argv) {
    parse_args(argc, argv);

    // check for input file
    fputs("DEBUG: Input file check\n", debugout);
    if (infile_path == NULL) {
        fputs("No input file specified\n", stderr);
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
        fprintf(stderr, "Input file '%s' does not exist or cannot be opened\n", infile_path);
        exit(1);
    }

    fprintf(debugout, "DEBUG: Input file: %s\n", infile_path);

    // read the file into a new lmcx struct
    fputs("DEBUG: Read input file\n", debugout);
    lmcx_file_descriptor_st *lmcx = read_lmcx_file(infile_path);

    if (lmcx == NULL) {
        fprintf(stderr, "Error: Failed to read input file '%s'\n", infile_path);
        exit(1);
    }

    fprintf(debugout, "DEBUG: Input file lmxm-ext version: %u\n", lmcx->ext_version);
    if (lmcx->ext_version > EXT_SUPPORTED_VERSION) {
        fprintf(stderr, "Error: Input file '%s' is of a newer lmvm-ext version than this VM supports\n", infile_path);
        exit(1);
    }

    fputs("DEBUG: Free lmcx data\n", debugout);
    free(lmcx->data);

    fputs("DEBUG: Free lmcx\n", debugout);
    free(lmcx);

    return 0;
}
