#include "common/file_io.h"
#include "common/executable_props.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/stat.h>

static int is_little_endian_machine = -1;
static void detect_little_endian_machine(void) {
    if (is_little_endian_machine != -1) {
        return;
    }

    const unsigned int x = 1;
    char *c = (char*) &x;
    is_little_endian_machine = (int) *c;

    // TODO: print below in debug mode only
    // printf("Detected machine endianness: %s\n", is_little_endian_machine ? "little" : "big");
}

lmcx_file_descriptor_st *read_lmcx_file(char *path) {
    detect_little_endian_machine();

    FILE *file = fopen(path, "rb");

    if (file == NULL) {
        return NULL;
    }

    // get the file size
    fseek(file, 0, SEEK_END);
    size_t file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    // read enough bytes to get either magic string
    size_t max_magic_length = strlen(MAGIC_STRING_LMC_EXTENDED);
    char *read_magic_string = malloc(sizeof(char) * max_magic_length);
    fread(read_magic_string, sizeof(char), max_magic_length, file);

    // check if the magic string is the extended one
    if (strcmp(read_magic_string, MAGIC_STRING_LMC_EXTENDED) == 0) {
        // read the supported ext version with correct endianness (file always stored in little endian)
        unsigned short int ext_version;
        fread(&ext_version, sizeof(unsigned short int), 1, file);
        if (!is_little_endian_machine) {
            ext_version = (ext_version << 8) | (ext_version >> 8);
        }

        // read the rest of the file with correct endianness (file always stored in little endian)
        size_t remaining_file_size = file_size - max_magic_length - sizeof(unsigned short int);
        unsigned short int *data = malloc(sizeof(unsigned short int) * remaining_file_size);
        fread(data, sizeof(unsigned short int), remaining_file_size, file);
        if (!is_little_endian_machine) {
            for (size_t i = 0; i < remaining_file_size; i++) {
                data[i] = (data[i] << 8) | (data[i] >> 8);
            }
        }

        fclose(file);


        // create the result
        lmcx_file_descriptor_st *result = malloc(sizeof(lmcx_file_descriptor_st));
        result->data = data;
        result->data_size = remaining_file_size;
        result->ext_version = ext_version;

        return result;
    }

    // trim read_magic_string to the length of MAGIC_STRING_LMC for comparison
    read_magic_string[strlen(MAGIC_STRING_LMC)] = '\0';

    // check if the magic string is the standard one
    if (strcmp(read_magic_string, MAGIC_STRING_LMC) == 0) {
        // read the rest of the file
        size_t remaining_file_size = file_size - max_magic_length;
        unsigned short int *data = malloc(sizeof(unsigned short int) * remaining_file_size);
        fread(data, sizeof(unsigned short int), remaining_file_size, file);

        fclose(file);

        // create the result
        lmcx_file_descriptor_st *result = malloc(sizeof(lmcx_file_descriptor_st));
        result->data = data;
        result->data_size = remaining_file_size;
        result->ext_version = 0;

        return result;
    }

    // close the file and return null if the magic string is not valid
    fclose(file);
    return NULL;
}

char *read_text_file(char *path) {
    FILE *file = fopen(path, "rb");

    if (file == NULL) {
        return NULL;
    }

    // get the file size
    fseek(file, 0, SEEK_END);
    unsigned int file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    // read the file
    char *data = malloc(sizeof(char) * (file_size + 1));
    fread(data, sizeof(char), file_size, file);
    data[file_size] = '\0';

    fclose(file);

    return data;
}


write_status_et write_lmcx_file(lmcx_file_descriptor_st *lmcx, char *path, int overwrite) {
    detect_little_endian_machine();

    FILE *file = fopen(path, "rb");

    // check if the file exists
    if (file != NULL) {
        fclose(file);

        if (!overwrite) {
            return WRITE_REFUSING_TO_OVERWRITE;
        }
    }

    file = fopen(path, "wb");

    if (file == NULL) {
        return WRITE_FAILURE;
    }

    // determine if using lmvm-ext
    int using_lmvm_ext = lmcx->ext_version != 0;

    // write the magic string
    if (using_lmvm_ext) {
        fwrite(MAGIC_STRING_LMC_EXTENDED, sizeof(char), strlen(MAGIC_STRING_LMC_EXTENDED), file);

        // write the version with correct endianness (file always stored in little endian)
        unsigned short int version = lmcx->ext_version;

        if (!is_little_endian_machine) {
            version = (version << 8) | (version >> 8);
        }

        fwrite(&version, sizeof(unsigned short int), 1, file);
    } else {
        fwrite(MAGIC_STRING_LMC, sizeof(char), strlen(MAGIC_STRING_LMC), file);
    }

    // write the data with correct endianness (file always stored in little endian)
    if (is_little_endian_machine) {
        fwrite(lmcx->data, sizeof(unsigned short int), lmcx->data_size, file);
    } else {
        unsigned short int *data = malloc(sizeof(unsigned short int) * lmcx->data_size);
        for (size_t i = 0; i < lmcx->data_size; i++) {
            data[i] = (lmcx->data[i] << 8) | (lmcx->data[i] >> 8);
        }
        fwrite(data, sizeof(unsigned short int), lmcx->data_size, file);
    }

    fclose(file);

    return WRITE_SUCCESS;
}

write_status_et write_text_file(char *data, char *path, int overwrite) {
    FILE *file = fopen(path, "rb");

    // check if the file exists
    if (file != NULL) {
        fclose(file);

        if (!overwrite) {
            return WRITE_REFUSING_TO_OVERWRITE;
        }
    }

    file = fopen(path, "wb");

    if (file == NULL) {
        return WRITE_FAILURE;
    }

    // write the data
    fwrite(data, sizeof(char), strlen(data), file);

    fclose(file);

    return WRITE_SUCCESS;
}


int file_exists_and_accessible(char *path) {
    FILE *file = fopen(path, "rb");

    if (file == NULL) {
        return 0;
    }

    fclose(file);
    return 1;
}

int is_dir(char *path) {
    struct stat statbuf;

    if (stat(path, &statbuf) != 0) {
        return 0;
    }

    return S_ISDIR(statbuf.st_mode);
}
