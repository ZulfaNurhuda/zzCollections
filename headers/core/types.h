#ifndef ZZ_TYPES_H
#define ZZ_TYPES_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

/* =========================================================
 * Callback Function Types
 * ========================================================= */

/**
 * @brief Function to free/cleanup an element
 *
 * This callback is invoked when an element needs to be deallocated
 * or cleaned up. Commonly used for freeing dynamically allocated
 * memory within elements.
 *
 * @param elem Pointer to element to free
 */
typedef void (*zzFreeFn)(void*);

/**
 * @brief Function to compare two elements
 *
 * Comparison function used for sorting and ordering operations
 * in tree-based and priority queue structures.
 *
 * @param a First element to compare
 * @param b Second element to compare
 * @return Negative value if a < b, zero if a == b, positive if a > b
 */
typedef int (*zzCompareFn)(const void* a, const void* b);

/**
 * @brief Function to hash a key
 *
 * Hash function used to compute hash values for keys in hash-based
 * data structures (HashMap, HashSet, etc.).
 *
 * @param key Key to hash
 * @return 32-bit hash value
 */
typedef uint32_t (*zzHashFn)(const void* key);

/**
 * @brief Function to check equality of two keys
 *
 * Equality function used in hash-based structures to determine if
 * two keys are equal (after hash collision).
 *
 * @param a First key to compare
 * @param b Second key to compare
 * @return true if keys are equal, false otherwise
 */
typedef bool (*zzEqualsFn)(const void* a, const void* b);

/**
 * @brief Function to iterate over elements
 *
 * Callback function used in forEach operations to process each
 * element in a collection.
 *
 * @param element Current element being visited
 * @param userdata User-provided context data passed through iteration
 */
typedef void (*zzForEachFn)(void* element, void* userdata);

#endif // ZZ_TYPES_H
