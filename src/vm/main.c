#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>
#include "common/file_io.h"
#include "common/executable_props.h"
#include "vm/execution.h"

// TODO: consider moving some parsing to common
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

// TODO: should these be in a scope and just passed to do_execution? does it matter?

// SPECIAL REGISTERS
static unsigned short int reg_PC = 0; // program counter
static int reg_ACC = 0; // accumulator
static unsigned short int reg_CIR = 0; // current instruction register
static unsigned short int reg_MAR = 0; // memory address register
// MDR isn't needed since we can simply access memory[reg_MAR] directly

// MEMORY ARRAY
static unsigned short int memory[EXECUTABLE_SIZE] = {0};


// returns 0 if execution was successful, 1 if there was an error
int do_execution(void) {
    execution_result_et result = EXECUTION_INDETERMINATE;

    while (result != EXECUTION_HALT && result != EXECUTION_ERROR) {
        // fetch
        // a real computer would go via the MAR and MDR, but we can go straight from RAM to CIR
        // TODO: CIR can be reduced down to scoped variable (instruction)
        reg_CIR = memory[reg_PC];

        fprintf(debugout, "DEBUG: CIR = %u\n", reg_CIR);


        // decode
        lmc_opcode_et opcode = (lmc_opcode_et) (reg_CIR / 100);
        // TODO: MAR can be reduced down to scoped variable (operand)
        reg_MAR = reg_CIR % 100;

        // check operand in range
        if (reg_MAR > 99) {
            fprintf(stderr, "Error: Operand out of range: %u\n", reg_MAR);
            result = EXECUTION_ERROR;
        }

        // if opcode is IO_OP, check for full valid operation (INP: 901, OUT: 902)
        if (opcode == OP_LMC_IO_OP) {
            if (reg_CIR == 901) {
                opcode = OP_LMC_IO_OP_INP;
            } else if (reg_CIR == 902) {
                opcode = OP_LMC_IO_OP_OUT;
            } else {
                fprintf(stderr, "Error: Invalid IO operation: %u\n", reg_CIR);
                result = EXECUTION_ERROR;
            }
        }

        fprintf(debugout, "DEBUG: Opcode = %u, Operand = %u\n", opcode, reg_MAR);


        // execute
        if (result != EXECUTION_ERROR) {
            result = execute(opcode, &reg_MAR, &reg_ACC, &reg_PC, memory);
        }

        fprintf(debugout, "DEBUG: Result = %u\n", result);


        // execute should never return EXECUTION_INDETERMINATE
        if (result == EXECUTION_INDETERMINATE) {
            result = EXECUTION_ERROR;
        }

        if (result == EXECUTION_ERROR) {
            fprintf(stderr, "Error occurred with PC = %u CIR = %u\n", reg_PC, reg_CIR);
        }
    }

    return result == EXECUTION_ERROR;
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

    fputs("DEBUG: Check data size\n", debugout);

    // TODO: should we separate value size and data count in the lmcx struct?
    unsigned int data_value_count = lmcx->data_size / sizeof(unsigned short int);

    if (data_value_count > EXECUTABLE_SIZE) {
        fprintf(stderr, "Error: Input file '%s' is too large to fit in memory (expected %u values but got %u)\n", infile_path, EXECUTABLE_SIZE, data_value_count);
        exit(1);
    }

    fputs("DEBUG: Load into memory\n", debugout);
    memcpy(memory, lmcx->data, EXECUTABLE_SIZE);


    fputs("DEBUG: Free lmcx data\n", debugout);
    free(lmcx->data);

    fputs("DEBUG: Free lmcx\n", debugout);
    free(lmcx);


    fputs("DEBUG: Start execution\n", debugout);

    int exit_code = do_execution();
    fprintf(debugout, "DEBUG: Execution finished with exit code %d\n", exit_code);

    return exit_code;
}
