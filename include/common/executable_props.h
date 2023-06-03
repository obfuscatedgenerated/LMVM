#ifndef LMVM_EXECUTABLE_PROPS_H
#define LMVM_EXECUTABLE_PROPS_H

#define EXECUTABLE_SIZE 100

#define LMC_ADD_PREFIX 1
#define LMC_SUB_PREFIX 2
#define LMC_STA_PREFIX 3
// 4xx is unused
#define LMC_LDA_PREFIX 5
#define LMC_BRA_PREFIX 6
#define LMC_BRZ_PREFIX 7
#define LMC_BRP_PREFIX 8
#define LMC_INP 901
#define LMC_OUT 902
#define LMC_HLT 000

// standard lmc files will start with LMCX, followed by the data
// extended lmc files will start with LMCXTENDED, followed by the major version, minor version, patch version, and the data
// both have file extensions of .lmc

#define DEFAULT_ASMFILE_EXT ".lmasm"
#define DEFAULT_EXECFILE_EXT ".lmc"

#define MAGIC_STRING_LMC "LMCX"
#define MAGIC_STRING_LMC_EXTENDED "LMCXTENDED"

#endif //LMVM_EXECUTABLE_PROPS_H
