/**
 * @file utils.h
 * @brief Utility functions for common operations
 *
 * Provides default implementations of hash and equality functions
 * for common data types (pointers, integers, strings).
 */

#ifndef ZZ_UTILS_H
#define ZZ_UTILS_H

#include "types.h"

/* =========================================================
 * Default Hash and Equality Functions
 * ========================================================= */

/**
 * @brief Default hash for pointers (uses address)
 *
 * Simple hash function that uses the pointer address directly
 * as the hash value. Suitable for pointer-based keys.
 *
 * @param key Pointer to hash
 * @return Hash value based on pointer address
 */
uint32_t zzDefaultHash(const void *key);

/**
 * @brief Default equality check (pointer comparison)
 *
 * Compares two pointers directly for equality.
 *
 * @param a First pointer
 * @param b Second pointer
 * @return true if pointers are equal, false otherwise
 */
bool zzDefaultEquals(const void *a, const void *b);

/**
 * @brief Hash function for int values
 *
 * Computes a hash value for integer data using a mixing algorithm
 * for good distribution. The input should be a pointer to int.
 *
 * @param key Pointer to int value (int*)
 * @return Hash value of the integer
 */
uint32_t zzIntHash(const void *key);

/**
 * @brief Equality check for int values
 *
 * Compares two integer values for equality. Both parameters
 * should be pointers to int.
 *
 * @param a Pointer to first int (int*)
 * @param b Pointer to second int (int*)
 * @return true if values are equal, false otherwise
 */
bool zzIntEquals(const void *a, const void *b);

/**
 * @brief FNV-1a hash for null-terminated strings
 *
 * Implements the FNV-1a hash algorithm for string hashing.
 * Provides good distribution for string keys. The input should
 * be a pointer to char pointer (char**).
 *
 * @param key Pointer to string pointer (char**)
 * @return FNV-1a hash value
 */
uint32_t zzStringHash(const void *key);

/**
 * @brief String equality check
 *
 * Compares two null-terminated strings for equality using strcmp.
 * Both parameters should be pointers to char pointers (char**).
 *
 * @param a Pointer to first string (char**)
 * @param b Pointer to second string (char**)
 * @return true if strings are equal (strcmp == 0), false otherwise
 */
bool zzStringEquals(const void *a, const void *b);

#endif // ZZ_UTILS_H
