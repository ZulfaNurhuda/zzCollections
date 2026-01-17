/**
 * @file utils.c
 * @brief Implementation of utility functions for common operations in the zzCollections library.
 *
 * This module provides implementations for utility functions for hashing, equality checking, and
 * comparison operations that are commonly used across various data structures in the
 * zzCollections library. These functions serve as default implementations for primitive
 * data types and can be overridden by users for custom types.
 */

#include "utils.h"
#include <string.h>

/**
 * @brief Computes a default hash for a pointer value.
 *
 * This function computes a hash value from a pointer by casting it to an integer.
 * It's intended as a default hash function for pointer-based keys when no
 * specific hash function is provided.
 *
 * @param key Pointer to hash
 * @return 32-bit hash value derived from the pointer address
 */
uint32_t zzDefaultHash(const void *key) {
    return (uint32_t)(uintptr_t)key;
}

/**
 * @brief Checks equality of two pointers.
 *
 * This function checks if two pointers are equal by comparing their addresses.
 * It's intended as a default equality function for pointer-based keys when no
 * specific equality function is provided.
 *
 * @param a First pointer to compare
 * @param b Second pointer to compare
 * @return true if both pointers refer to the same address, false otherwise
 */
bool zzDefaultEquals(const void *a, const void *b) {
    return a == b;
}

/**
 * @brief Compares two pointers.
 *
 * This function compares two pointers by casting them to integers and comparing
 * their values. It's intended as a default comparison function for pointer-based
 * keys when no specific comparison function is provided.
 *
 * @param a First pointer to compare
 * @param b Second pointer to compare
 * @return Negative value if a < b, zero if a == b, positive value if a > b
 */
int zzDefaultCompare(const void *a, const void* b) {
    uintptr_t ua = (uintptr_t)a, ub = (uintptr_t)b;
    return (ua > ub) - (ua < ub);
}

/**
 * @brief Computes a hash for a character value.
 *
 * This function computes a hash value from a character by casting it to an integer.
 * It's designed for use with character-based keys in hash tables and similar structures.
 *
 * @param key Pointer to the character to hash
 * @return 32-bit hash value derived from the character
 */
uint32_t zzCharHash(const void *key) {
    return (uint32_t)(*(const char*)key);
}

/**
 * @brief Checks equality of two characters.
 *
 * This function checks if two characters are equal by comparing their values.
 * It's designed for use with character-based keys in hash tables and similar structures.
 *
 * @param a Pointer to the first character to compare
 * @param b Pointer to the second character to compare
 * @return true if both characters have the same value, false otherwise
 */
bool zzCharEquals(const void *a, const void *b) {
    return *(const char*)a == *(const char*)b;
}

/**
 * @brief Compares two characters.
 *
 * This function compares two characters by comparing their values.
 * It's designed for use with character-based keys in ordered structures like trees.
 *
 * @param a Pointer to the first character to compare
 * @param b Pointer to the second character to compare
 * @return Negative value if *a < *b, zero if *a == *b, positive value if *a > *b
 */
int zzCharCompare(const void *a, const void *b) {
    char ca = *(const char*)a, cb = *(const char*)b;
    return (ca > cb) - (ca < cb);
}

/**
 * @brief Computes a hash for an integer value.
 *
 * This function computes a hash value from an integer using a simple mixing algorithm
 * to distribute values evenly across hash table buckets. It's designed for use with
 * integer-based keys in hash tables and similar structures.
 *
 * @param key Pointer to the integer to hash
 * @return 32-bit hash value derived from the integer with improved distribution
 */
uint32_t zzIntHash(const void *key) {
    uint32_t h = (uint32_t)*(const int*)key;
    h ^= h >> 16;
    h *= 0x85ebca6b;
    h ^= h >> 13;
    h *= 0xc2b2ae35;
    h ^= h >> 16;
    return h;
}

/**
 * @brief Checks equality of two integers.
 *
 * This function checks if two integers are equal by comparing their values.
 * It's designed for use with integer-based keys in hash tables and similar structures.
 *
 * @param a Pointer to the first integer to compare
 * @param b Pointer to the second integer to compare
 * @return true if both integers have the same value, false otherwise
 */
bool zzIntEquals(const void *a, const void *b) {
    return *(const int*)a == *(const int*)b;
}

/**
 * @brief Compares two integers.
 *
 * This function compares two integers by comparing their values.
 * It's designed for use with integer-based keys in ordered structures like trees.
 *
 * @param a Pointer to the first integer to compare
 * @param b Pointer to the second integer to compare
 * @return Negative value if *a < *b, zero if *a == *b, positive value if *a > *b
 */
int zzIntCompare(const void *a, const void *b) {
    int ia = *(const int*)a, ib = *(const int*)b;
    return (ia > ib) - (ia < ib);
}

/**
 * @brief Computes a hash for a long integer value.
 *
 * This function computes a hash value from a long integer using a more complex mixing algorithm
 * to distribute values evenly across hash table buckets. It's designed for use with
 * long integer-based keys in hash tables and similar structures.
 *
 * @param key Pointer to the long integer to hash
 * @return 32-bit hash value derived from the long integer with improved distribution
 */
uint32_t zzLongHash(const void *key) {
    uint64_t h = (uint64_t)*(const long*)key;
    h ^= h >> 33;
    h *= 0xff51afd7ed558ccdULL;
    h ^= h >> 33;
    h *= 0xc4ceb9fe1a85ec53ULL;
    h ^= h >> 33;
    return (uint32_t)h;
}

/**
 * @brief Checks equality of two long integers.
 *
 * This function checks if two long integers are equal by comparing their values.
 * It's designed for use with long integer-based keys in hash tables and similar structures.
 *
 * @param a Pointer to the first long integer to compare
 * @param b Pointer to the second long integer to compare
 * @return true if both long integers have the same value, false otherwise
 */
bool zzLongEquals(const void *a, const void *b) {
    return *(const long*)a == *(const long*)b;
}

/**
 * @brief Compares two long integers.
 *
 * This function compares two long integers by comparing their values.
 * It's designed for use with long integer-based keys in ordered structures like trees.
 *
 * @param a Pointer to the first long integer to compare
 * @param b Pointer to the second long integer to compare
 * @return Negative value if *a < *b, zero if *a == *b, positive value if *a > *b
 */
int zzLongCompare(const void *a, const void *b) {
    long la = *(const long*)a, lb = *(const long*)b;
    return (la > lb) - (la < lb);
}

/**
 * @brief Computes a hash for a float value.
 *
 * This function computes a hash value from a float by converting its bit representation
 * to an integer. It's designed for use with float-based keys in hash tables and similar structures.
 *
 * @param key Pointer to the float to hash
 * @return 32-bit hash value derived from the float's bit representation
 */
uint32_t zzFloatHash(const void *key) {
    uint32_t bits;
    memcpy(&bits, key, sizeof(uint32_t));
    return bits;
}

/**
 * @brief Checks equality of two float values.
 *
 * This function checks if two floats are equal by comparing their values.
 * It's designed for use with float-based keys in hash tables and similar structures.
 *
 * @param a Pointer to the first float to compare
 * @param b Pointer to the second float to compare
 * @return true if both floats have the same value, false otherwise
 */
bool zzFloatEquals(const void *a, const void *b) {
    return *(const float*)a == *(const float*)b;
}

/**
 * @brief Compares two float values.
 *
 * This function compares two float values by comparing their numeric values.
 * It's designed for use with float-based keys in ordered structures like trees.
 *
 * @param a Pointer to the first float to compare
 * @param b Pointer to the second float to compare
 * @return Negative value if *a < *b, zero if *a == *b, positive value if *a > *b
 */
int zzFloatCompare(const void *a, const void *b) {
    float fa = *(const float*)a, fb = *(const float*)b;
    return (fa > fb) - (fa < fb);
}

/**
 * @brief Computes a hash for a double value.
 *
 * This function computes a hash value from a double by converting its bit representation
 * to an integer. It's designed for use with double-based keys in hash tables and similar structures.
 *
 * @param key Pointer to the double to hash
 * @return 32-bit hash value derived from the double's bit representation
 */
uint32_t zzDoubleHash(const void *key) {
    uint64_t bits;
    memcpy(&bits, key, sizeof(uint64_t));
    return (uint32_t)(bits ^ (bits >> 32));
}

/**
 * @brief Checks equality of two double values.
 *
 * This function checks if two doubles are equal by comparing their values.
 * It's designed for use with double-based keys in hash tables and similar structures.
 *
 * @param a Pointer to the first double to compare
 * @param b Pointer to the second double to compare
 * @return true if both doubles have the same value, false otherwise
 */
bool zzDoubleEquals(const void *a, const void *b) {
    return *(const double*)a == *(const double*)b;
}

/**
 * @brief Compares two double values.
 *
 * This function compares two double values by comparing their numeric values.
 * It's designed for use with double-based keys in ordered structures like trees.
 *
 * @param a Pointer to the first double to compare
 * @param b Pointer to the second double to compare
 * @return Negative value if *a < *b, zero if *a == *b, positive value if *a > *b
 */
int zzDoubleCompare(const void *a, const void *b) {
    double da = *(const double*)a, db = *(const double*)b;
    return (da > db) - (da < db);
}

/**
 * @brief Computes a hash for a string value using FNV-1a algorithm.
 *
 * This function computes a hash value from a null-terminated string using the
 * FNV-1a (Fowler-Noll-Vo) hash algorithm, which provides good distribution
 * properties for string keys in hash tables.
 *
 * @param key Pointer to the null-terminated string to hash
 * @return 32-bit hash value computed using the FNV-1a algorithm
 */
uint32_t zzStringHash(const void *key) {
    const char *str = *(const char**)key;
    uint32_t hash = 2166136261u;
    while (*str) {
        hash = (hash ^ (uint8_t)*str++) * 16777619;
    }
    return hash;
}

/**
 * @brief Checks equality of two strings.
 *
 * This function checks if two null-terminated strings are equal by comparing
 * their contents using strcmp. It's designed for use with string-based keys
 * in hash tables and similar structures.
 *
 * @param a Pointer to the first string to compare
 * @param b Pointer to the second string to compare
 * @return true if both strings have identical content, false otherwise
 */
bool zzStringEquals(const void *a, const void *b) {
    return strcmp(*(const char**)a, *(const char**)b) == 0;
}

/**
 * @brief Compares two string values.
 *
 * This function compares two null-terminated strings lexicographically using strcmp.
 * It's designed for use with string-based keys in ordered structures like trees.
 *
 * @param a Pointer to the first string to compare
 * @param b Pointer to the second string to compare
 * @return Negative value if string a < string b, zero if a == b, positive value if a > b
 */
int zzStringCompare(const void *a, const void *b) {
    return strcmp(*(const char**)a, *(const char**)b);
}