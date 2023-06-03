#ifndef FNV1A_HASHTABLE_FNV1A_H
#define FNV1A_HASHTABLE_FNV1A_H

#include <stdint.h>
#include <stddef.h>

#define FNV1A_BASIS 0xcbf29ce484222325
#define FNV1A_PRIME 0x100000001b3

uint64_t fnv1a(const void *payload, size_t len);

#endif //FNV1A_HASHTABLE_FNV1A_H
