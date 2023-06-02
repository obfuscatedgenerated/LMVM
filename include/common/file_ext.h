#ifndef LMVM_FILE_EXT_H
#define LMVM_FILE_EXT_H

// standard lmc files will start with LMCX, followed by the data
// extended lmc files will start with LMCXTENDED, followed by the major version, minor version, patch version, and the data
// both have file extensions of .lmc

#define FILE_EXT_LMC "lmc"

#define MAGIC_STRING_LMC "LMCX"
#define MAGIC_STRING_LMC_EXTENDED "LMCXTENDED"

#endif //LMVM_FILE_EXT_H
