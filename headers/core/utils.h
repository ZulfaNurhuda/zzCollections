/**
 * @file utils.h
 * @brief Utility functions for common operations
 *
 * Provides default implementations of hash and equality functions
 * for common data types (pointers, integers, strings, primitives).
 */

#ifndef ZZ_UTILS_H
#define ZZ_UTILS_H

#include "types.h"

/* =========================================================
 * Default Hash and Equality Functions
 * ========================================================= */

/**
 * @brief Default hash for pointers (uses address)
 * @param key Pointer to hash
 * @return Hash value based on pointer address
 */
uint32_t zzDefaultHash(const void *key);

/**
 * @brief Default equality check (pointer comparison)
 * @param a First pointer
 * @param b Second pointer
 * @return true if pointers are equal, false otherwise
 */
bool zzDefaultEquals(const void *a, const void *b);

/* =========================================================
 * Integer Type Hash and Equality Functions
 * ========================================================= */

/**
 * @brief Hash function for char values
 * @param key Pointer to char value (char*)
 * @return Hash value of the char
 */
uint32_t zzCharHash(const void *key);

/**
 * @brief Equality check for char values
 * @param a Pointer to first char (char*)
 * @param b Pointer to second char (char*)
 * @return true if values are equal, false otherwise
 */
bool zzCharEquals(const void *a, const void *b);

/**
 * @brief Hash function for int values
 * @param key Pointer to int value (int*)
 * @return Hash value of the integer
 */
uint32_t zzIntHash(const void *key);

/**
 * @brief Equality check for int values
 * @param a Pointer to first int (int*)
 * @param b Pointer to second int (int*)
 * @return true if values are equal, false otherwise
 */
bool zzIntEquals(const void *a, const void *b);

/**
 * @brief Hash function for long values
 * @param key Pointer to long value (long*)
 * @return Hash value of the long
 */
uint32_t zzLongHash(const void *key);

/**
 * @brief Equality check for long values
 * @param a Pointer to first long (long*)
 * @param b Pointer to second long (long*)
 * @return true if values are equal, false otherwise
 */
bool zzLongEquals(const void *a, const void *b);

/* =========================================================
 * Floating Point Hash and Equality Functions
 * ========================================================= */

/**
 * @brief Hash function for float values
 * @param key Pointer to float value (float*)
 * @return Hash value of the float
 */
uint32_t zzFloatHash(const void *key);

/**
 * @brief Equality check for float values
 * @param a Pointer to first float (float*)
 * @param b Pointer to second float (float*)
 * @return true if values are equal, false otherwise
 */
bool zzFloatEquals(const void *a, const void *b);

/**
 * @brief Hash function for double values
 * @param key Pointer to double value (double*)
 * @return Hash value of the double
 */
uint32_t zzDoubleHash(const void *key);

/**
 * @brief Equality check for double values
 * @param a Pointer to first double (double*)
 * @param b Pointer to second double (double*)
 * @return true if values are equal, false otherwise
 */
bool zzDoubleEquals(const void *a, const void *b);

/* =========================================================
 * String Hash and Equality Functions
 * ========================================================= */

/**
 * @brief FNV-1a hash for null-terminated strings
 * @param key Pointer to string pointer (char**)
 * @return FNV-1a hash value
 */
uint32_t zzStringHash(const void *key);

/**
 * @brief String equality check
 * @param a Pointer to first string (char**)
 * @param b Pointer to second string (char**)
 * @return true if strings are equal, false otherwise
 */
bool zzStringEquals(const void *a, const void *b);

#endif /* ZZ_UTILS_H */
