/**
 * @file types.h
 * @brief Common type definitions used throughout the zzCollections library.
 *
 * This module defines common types, function pointers, and enumerations
 * that are used across various data structures in the zzCollections library.
 * These types provide a consistent interface for comparison, hashing, and
 * memory management operations.
 */

#ifndef ZZ_TYPES_H
#define ZZ_TYPES_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

/**
 * @brief Enumeration representing colors for red-black tree nodes.
 *
 * This enumeration is used in red-black trees to maintain balance properties.
 * Each node is either red or black, and these colors help enforce the tree's
 * balancing rules during insertion and deletion operations.
 */
typedef enum { ZZ_RED, ZZ_BLACK } zzRBColor;

/**
 * @brief Function pointer type for freeing memory.
 *
 * This function pointer type represents a function that takes a void pointer
 * and frees the memory it points to. It's commonly used for custom memory
 * management in data structures that store dynamically allocated elements.
 *
 * @param ptr Pointer to the memory to be freed
 */
typedef void (*zzFreeFn)(void*);

/**
 * @brief Function pointer type for comparing two values.
 *
 * This function pointer type represents a comparison function that takes two
 * constant void pointers and returns an integer indicating their relative order.
 * The return value follows the convention:
 * - Negative if the first value is less than the second
 * - Zero if the values are equal
 * - Positive if the first value is greater than the second
 *
 * @param a Pointer to the first value to compare
 * @param b Pointer to the second value to compare
 * @return Integer indicating the relative order of the values
 */
typedef int (*zzCompareFn)(const void* a, const void* b);

/**
 * @brief Function pointer type for computing hash values.
 *
 * This function pointer type represents a hash function that takes a constant
 * void pointer and returns a 32-bit hash value. It's used in hash-based data
 * structures to determine where elements should be stored.
 *
 * @param key Pointer to the key to be hashed
 * @return 32-bit hash value computed from the key
 */
typedef uint32_t (*zzHashFn)(const void* key);

/**
 * @brief Function pointer type for checking equality of two values.
 *
 * This function pointer type represents an equality function that takes two
 * constant void pointers and returns a boolean indicating whether they are equal.
 * It's used in hash-based and tree-based data structures to determine if keys
 * or values match.
 *
 * @param a Pointer to the first value to compare
 * @param b Pointer to the second value to compare
 * @return true if the values are equal, false otherwise
 */
typedef bool (*zzEqualsFn)(const void* a, const void* b);

/**
 * @brief Function pointer type for applying a function to each element.
 *
 * This function pointer type represents a function that takes an element and
 * user-defined data, and performs some operation on the element. It's commonly
 * used in iteration operations across various data structures.
 *
 * @param element Pointer to the current element being processed
 * @param userdata Pointer to user-defined data that may be used in the operation
 */
typedef void (*zzForEachFn)(void* element, void* userdata);

#endif