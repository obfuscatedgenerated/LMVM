#include "common/file_io.h"
#include "common/executable_props.h"
#include "common/checked_alloc.h"

#include <string.h>
#include <stdio.h>
#include <sys/stat.h>

static int is_little_endian_machine = -1;

static void detect_little_endian_machine(void) {
    if (is_little_endian_machine != -1) {
        return;
    }

    const unsigned int x = 1;
    char *c = (char *) &x;
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

    // read enough bytes to get the largest magic string, checking success
    size_t ext_magic_string_length = strlen(MAGIC_STRING_LMC_EXTENDED);
    char *read_magic_string = checked_malloc(sizeof(char) * ext_magic_string_length);
    if (fread(read_magic_string, sizeof(char), ext_magic_string_length, file) != ext_magic_string_length) {
        // close the file and return null if the magic string is not valid
        checked_free(read_magic_string);
        fclose(file);
        return NULL;
    }

    unsigned short int ext_version;
    size_t remaining_file_size;
    int found_magic_string = 0;

    // check if the magic string is the extended one
    if (strcmp(read_magic_string, MAGIC_STRING_LMC_EXTENDED) == 0) {
        found_magic_string = 1;

        // read the supported ext version with correct endianness (file always stored in little endian), checking success
        if (fread(&ext_version, sizeof(unsigned short int), 1, file) != sizeof (unsigned short int)) {
            // close the file and return null if the magic string is not valid
            checked_free(read_magic_string);
            fclose(file);
            return NULL;
        }
        if (!is_little_endian_machine) {
            ext_version = (ext_version << 8) | (ext_version >> 8);
        }

        // set the remaining file size
        remaining_file_size = file_size - ext_magic_string_length - sizeof(unsigned short int);
    }

    if (!found_magic_string) {
        // trim read_magic_string to the length of MAGIC_STRING_LMC for comparison
        size_t magic_string_lmc_length = strlen(MAGIC_STRING_LMC);
        read_magic_string[magic_string_lmc_length] = '\0';

        // check if the magic string is the standard one
        if (strcmp(read_magic_string, MAGIC_STRING_LMC) != 0) {
            // close the file and return null if the magic string is not valid
            checked_free(read_magic_string);
            fclose(file);
            return NULL;
        }

        // seek back to where the standard magic string ends for further reading
        fseek(file, (long) magic_string_lmc_length, SEEK_SET);

        // standard magic string found, set the ext version to 0 and set the remaining file size
        ext_version = 0;
        remaining_file_size = file_size - magic_string_lmc_length;
    }

    // read the rest of the file, checking success
    unsigned short int *data = checked_malloc(sizeof(unsigned short int) * remaining_file_size);
    if (fread(data, sizeof(unsigned short int), remaining_file_size, file) != remaining_file_size / sizeof(unsigned short int)) {
        // close the file and return null if the magic string is not valid
        checked_free(read_magic_string);
        checked_free(data);
        fclose(file);
        return NULL;
    }

    fclose(file);

    // create the result
    lmcx_file_descriptor_st *result = checked_malloc(sizeof(lmcx_file_descriptor_st));
    result->data = data;
    result->data_size = remaining_file_size;
    result->ext_version = 0;

    checked_free(read_magic_string);
    return result;
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

    // read the file, checking success
    char *data = checked_malloc(sizeof(char) * (file_size + 1));
    if (fread(data, sizeof(char), file_size, file) != file_size) {
        checked_free(data);
        fclose(file);
        return NULL;
    }
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
        unsigned short int *data = checked_malloc(sizeof(unsigned short int) * lmcx->data_size);
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
