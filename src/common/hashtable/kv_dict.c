#include "common/hashtable/kv_dict.h"
#include "common/hashtable/fnv1a.h"
#include "checked_alloc.h"

#include <stdlib.h>
#include <string.h>
#include <assert.h>

kv_dict *new_dict() {
    kv_dict *dict = checked_malloc(sizeof(kv_dict));

    dict->size = 0;
    dict->capacity = INIT_CAPACITY;

    // allocate entry space and zero it out
    dict->entries = checked_calloc(dict->capacity, sizeof(kv_entry));

    // check success
    if (dict->entries == NULL) {
        checked_free(dict);
        return NULL;
    }

    return dict;
}

void free_dict(kv_dict *dict) {
    // TODO: free each entry. ive spent like an hour trying to make it work
    // iterating each entry and freeing it causes a corruption error
    checked_free(dict->entries);
    checked_free(dict);
}

static void raw_set(kv_entry *entries, size_t capacity, void *key, size_t key_size, void *value) {
    // create a new entry
    kv_entry *entry = checked_malloc(sizeof(kv_entry));

    entry->key = key;
    entry->value = value;

    // hash the key à la fnv
    uint64_t hash = fnv1a(key, key_size);

    // use AND to encode hash in terms of our array's indices (guess at index)
    size_t index = hash & (capacity - 1);

    // find the closest free position
    while (entries[index].key != NULL) {
        // check if the key is the one we're setting
        if (strcmp(entries[index].key, key) == 0) {
            // same key, so update value and finish
            entries[index].value = value;
            return;
        }

        index++;

        // wrap around if need be
        if (index >= capacity) {
            index = 0;
        }
    }

    // insert the item
    entries[index] = *entry;
}


static void expand(kv_dict *dict) {
    size_t new_capacity = dict->capacity * 2;

    kv_entry *new_entries = checked_calloc(dict->capacity, sizeof(kv_entry));

    // migrate old non-null entries
    for (size_t i = 0; i < dict->capacity; i++) {
        kv_entry entry = dict->entries[i];

        if (entry.key != NULL) {
            raw_set(new_entries, new_capacity, entry.key, strlen(entry.key), entry.value);
        }
    }

    // free old entries
    checked_free(dict->entries);

    dict->entries = new_entries;
    dict->capacity = new_capacity;
}


void set_item(kv_dict *dict, void *key, size_t key_size, void *value) {
    // ensure data
    assert(value != NULL);

    // expand capacity if needed
    if (dict->size == dict->capacity) {
        expand(dict);
    }

    raw_set(dict->entries, dict->capacity, key, key_size, value);
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
