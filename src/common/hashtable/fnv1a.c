#include "common/hashtable/fnv1a.h"

uint64_t fnv1a(const void *payload, size_t len) {
    uint64_t hash = FNV1A_BASIS;

    for (size_t i = 0; i < len; i++) {
        hash ^= ((const uint8_t *)payload)[i];
        hash *= FNV1A_PRIME;
    }

    return hash;
}
