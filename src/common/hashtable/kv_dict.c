#include "common/hashtable/kv_dict.h"
#include "common/hashtable/fnv1a.h"

#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>

kv_dict *new_dict() {
    kv_dict *dict = malloc(sizeof(kv_dict));

    // check success
    if (dict == NULL) {
        return NULL;
    }

    dict->size = 0;
    dict->capacity = INIT_CAPACITY;

    // allocate entry space and zero it out
    dict->entries = calloc(dict->capacity, sizeof(kv_entry));

    // check success
    if (dict->entries == NULL) {
        free(dict);
        return NULL;
    }

    return dict;
}

void free_dict(kv_dict *dict) {
    free(dict->entries);
    free(dict);
}

void set_item(kv_dict *dict, void *key, size_t key_size, void *value) {
    // ensure data
    assert(value != NULL);

    // expand capacity if needed
    if (dict->size == dict->capacity) {
        dict->capacity *= 2;
        dict->entries = realloc(dict->entries, dict->capacity * sizeof(kv_entry));

        // ensure success
        assert(dict->entries != NULL);
    }

    // create a new entry
    kv_entry *entry = malloc(sizeof(kv_entry));

    // ensure success
    assert(entry != NULL);

    entry->key = key;
    entry->value = value;

    // hash the key à la fnv
    uint64_t hash = fnv1a(key, key_size);

    // use AND to encode hash in terms of our array's indices (guess at index)
    size_t index = hash & (dict->capacity - 1);

    // find the closest free position
    while (dict->entries[index].key != NULL) {
        // check if the key is the one we're setting
        if (strcmp(dict->entries[index].key, key) == 0) {
            // same key, so update value and finish
            dict->entries[index].value = value;
            return;
        }

        index++;

        // wrap around if need be
        if (index >= dict->capacity) {
            index = 0;
        }
    }

    // insert the item
    dict->entries[index] = *entry;
    dict->size++;
}

void *get_item(kv_dict *dict, void *key, size_t key_size) {
    // hash the key à la fnv
    uint64_t hash = fnv1a(key, key_size);

    // use AND to encode hash in terms of our array's indices (guess at index)
    size_t index = hash & (dict->capacity - 1);

    // find the actual entry
    while (dict->entries[index].key != NULL) {
        // check if the key is the one we're getting
        if (strcmp(dict->entries[index].key, key) == 0) {
            // got the entry! return the value
            return dict->entries[index].value;
        }

        index++;

        // wrap around if need be
        if (index >= dict->capacity) {
            index = 0;
        }
    }

    // wasn't found!
    return NULL;
}
