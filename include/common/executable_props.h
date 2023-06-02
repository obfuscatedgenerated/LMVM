#ifndef LMVM_EXECUTABLE_PROPS_H
#define LMVM_EXECUTABLE_PROPS_H

#define EXECUTABLE_SIZE 100

#define LMC_INP 901
#define LMC_OUT 902
#define LMC_HLT 000

// standard lmc files will start with LMCX, followed by the data
// extended lmc files will start with LMCXTENDED, followed by the major version, minor version, patch version, and the data
// both have file extensions of .lmc

#define DEFAULT_INFILE_EXT "lmasm"
#define DEFAULT_OUTFILE_EXT "lmc"

#define MAGIC_STRING_LMC "LMCX"
#define MAGIC_STRING_LMC_EXTENDED "LMCXTENDED"

#endif //LMVM_EXECUTABLE_PROPS_H
