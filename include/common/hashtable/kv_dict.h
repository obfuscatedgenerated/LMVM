#ifndef FNV1A_HASHTABLE_KV_DICT_H
#define FNV1A_HASHTABLE_KV_DICT_H

#include <stddef.h>

#define INIT_CAPACITY 8

struct kv_entry_s {
    void *key;
    void *value;
};

typedef struct kv_entry_s kv_entry;

struct kv_dict_s {
    kv_entry *entries;
    size_t size;
    size_t capacity;
};

typedef struct kv_dict_s kv_dict;

kv_dict* new_dict(void);

void free_dict(kv_dict *dict);

void set_item(kv_dict *dict, void *key, size_t key_size, void *value);

void *get_item(kv_dict *dict, void *key, size_t key_size);

#endif //FNV1A_HASHTABLE_KV_DICT_H
