#ifndef ZZ_TYPES_H
#define ZZ_TYPES_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

typedef void (*zzFreeFn)(void*);

typedef int (*zzCompareFn)(const void* a, const void* b);

typedef uint32_t (*zzHashFn)(const void* key);

typedef bool (*zzEqualsFn)(const void* a, const void* b);

typedef void (*zzForEachFn)(void* element, void* userdata);

#endif