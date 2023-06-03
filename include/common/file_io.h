#ifndef LMVM_FILE_IO_H
#define LMVM_FILE_IO_H

#include <stdio.h>

/**
 * Represents an LMCX file and metadata.
 * Contains data and the version of the lmvm-ext set used, listed {major,minor,patch} or {0,0,0} if it is a standard LMC file.
 * @see lmcx_file_descriptor_st
 */
struct lmcx_file_descriptor_s {
    unsigned short int *data;
    size_t data_size;
    unsigned short int ext_version;
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


/**
 * Writes an LMCX file from a descriptor to a path.
 * @see lmcx_file_descriptor_st
 *
 * @param lmcx       The lmcx to write
 * @param path       The path to write the file to
 * @param overwrite  Whether to overwrite the file if it exists
 * @return           The result of writing the file
 */
write_status_et write_lmcx_file(lmcx_file_descriptor_st *lmcx, char *path, int overwrite);

/**
 * Writes a text file from a string to a path.
 *
 * @param data       The data to write
 * @param path       The path to write the file to
 * @param overwrite  Whether to overwrite the file if it exists
 * @return           The result of writing the file
 */
write_status_et write_text_file(char *data, char *path, int overwrite);


/**
 * Checks if a file exists and is accessible.
 *
 * @param path  The path of the file to check
 * @return      Whether the file exists
 */
int file_exists_and_accessible(char *path);

/**
 * Checks if a path is a directory.
 *
 * @param path  The path to check
 * @return      Whether the path is a directory
 */
int is_dir(char *path);

#endif //LMVM_FILE_IO_H
