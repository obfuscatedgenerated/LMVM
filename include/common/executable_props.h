#ifndef LMVM_EXECUTABLE_PROPS_H
#define LMVM_EXECUTABLE_PROPS_H

#define EXECUTABLE_SIZE 100
#define EXT_SUPPORTED_VERSION 0

// standard lmc files will start with LMCX, followed by the data
// extended lmc files will start with LMCXTENDED, followed by the major version, minor version, patch version, and the data
// both have file extensions of .lmc

#define DEFAULT_ASMFILE_EXT ".lmasm"
#define DEFAULT_EXECFILE_EXT ".lmc"

#define MAGIC_STRING_LMC "LMCX"
#define MAGIC_STRING_LMC_EXTENDED "LMCXTENDED"

#endif //LMVM_EXECUTABLE_PROPS_H
