#ifndef LMVM_FILE_IO_H
#define LMVM_FILE_IO_H

#include <stdio.h>

/**
 * Represents an LMCX file and metadata.
 * Contains data and the version of the lmvm-ext set used, listed {major,minor,patch} or {0,0,0} if it is a standard LMC file.
 * @see lmcx_file_descriptor_st
 */
struct lmcx_file_descriptor_s {
    unsigned int *data;
    size_t data_size;
    unsigned int ext_version[3];
};

/**
 * Represents an LMCX file and metadata.
 * Contains data and the version of the lmvm-ext set used, listed {major,minor,patch} or {0,0,0} if it is a standard LMC file.
 * @see lmcx_file_descriptor_s
 */
typedef struct lmcx_file_descriptor_s lmcx_file_descriptor_st;


/**
 * Represents the result of writing a file.
 * @see write_status_et
 */
enum write_status_e {
    WRITE_SUCCESS,
    WRITE_REFUSING_TO_OVERWRITE,
    WRITE_FAILURE
};

/**
 * Represents the result of writing a file.
 * @see write_status_e
 */
typedef enum write_status_e write_status_et;


/**
 * Reads an LMCX file and returns the data and lmvm-ext version used, or NULL if the file is not valid or can't be opened.
 * @see lmcx_file_descriptor_st
 *
 * @param path  The path of the file to read
 * @return      The result of reading the file
 */
lmcx_file_descriptor_st *read_lmcx_file(char *path);

/**
 * Reads a text file and returns the data or NULL if the file can't be opened.
 *
 * @param path  The path of the file to read
 * @return      The data of the file
 */
char *read_text_file(char *path);

#endif //LMVM_FILE_IO_H
