#ifndef COMMON_H
#define COMMON_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

// ====================
// ==== TYPEDEFS & CALLBACKS ====
// ====================

/**
 * Function to free/cleanup an element
 * @param elem Pointer to element to free
 */
typedef void (*zzFreeFn)(void*);

/**
 * Function to compare two elements
 * @return <0 if a<b, 0 if a==b, >0 if a>b
 */
typedef int (*zzCompareFn)(const void*, const void*);

/**
 * Function to hash a key
 * @return Hash value
 */
typedef uint32_t (*zzHashFn)(const void*);

/**
 * Function to check equality of two keys
 * @return true if equal, false otherwise
 */
typedef bool (*zzEqualsFn)(const void*, const void*);

/**
 * Function to iterate over elements
 * @param element Current element
 * @param userdata User-provided data
 */
typedef void (*zzForEachFn)(void*, void*);

// ====================
// ==== DEFAULT HASH & EQUALS ====
// ====================

/**
 * Default hash for integers/pointers (uses pointer address)
 */
uint32_t zzDefaultHash(const void *key);

/**
 * Default equals (pointer comparison)
 */
bool zzDefaultEquals(const void *a, const void *b);

/**
 * Hash function for int values (expects int*)
 */
uint32_t zzIntHash(const void *key);

/**
 * Equality check for int values (expects int*)
 */
bool zzIntEquals(const void *a, const void *b);

/**
 * FNV-1a hash for strings (expects char**)
 */
uint32_t zzStringHash(const void *key);

/**
 * String equality check (expects char**)
 */
bool zzStringEquals(const void *a, const void *b);

#endif // COMMON_H
