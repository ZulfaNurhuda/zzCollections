/**
 * @file utils.c
 * @brief Utility functions for the zzCollections library
 *
 * Contains default implementations of hash and equality functions
 * for common data types, as well as helper functions for various operations.
 */

#include "utils.h"
#include <string.h>

/* =========================================================
 * Default Hash and Equality Functions
 * ========================================================= */

/**
 * @brief Default hash function for pointer addresses
 *
 * Computes a hash based on the pointer address itself.
 * This is a simple identity hash function that works well when
 * keys are unique pointers.
 *
 * @param[in] key Pointer to the key to hash
 * @return 32-bit hash value derived from the pointer address
 */
uint32_t zzDefaultHash(const void *key) {
    return (uint32_t)(uintptr_t)key;
}

/**
 * @brief Default equality function for pointer addresses
 *
 * Compares two pointers for equality based on their addresses.
 * This function checks if both pointers refer to the same memory location.
 *
 * @param[in] a First pointer to compare
 * @param[in] b Second pointer to compare
 * @return true if both pointers are identical, false otherwise
 */
bool zzDefaultEquals(const void *a, const void *b) {
    return a == b;
}

/* =========================================================
 * Integer Type Hash and Equality Functions
 * ========================================================= */

/**
 * @brief Hash function for character values
 *
 * Computes a hash based on the character value itself.
 * This is a simple identity hash function for single characters.
 *
 * @param[in] key Pointer to the character to hash
 * @return 32-bit hash value derived from the character value
 */
uint32_t zzCharHash(const void *key) {
    return (uint32_t)(*(const char*)key);
}

/**
 * @brief Equality function for character values
 *
 * Compares two characters for equality based on their values.
 *
 * @param[in] a First character to compare
 * @param[in] b Second character to compare
 * @return true if both characters have the same value, false otherwise
 */
bool zzCharEquals(const void *a, const void *b) {
    return *(const char*)a == *(const char*)b;
}

/**
 * @brief Hash function for integer values
 *
 * Computes a hash using a variant of the MurmurHash3 algorithm.
 * This provides good distribution for integer values.
 *
 * @param[in] key Pointer to the integer to hash
 * @return 32-bit hash value derived from the integer value
 */
uint32_t zzIntHash(const void *key) {
    int val = *(const int*)key;
    uint32_t h = (uint32_t)val;
    h ^= h >> 16;
    h *= 0x85ebca6b;
    h ^= h >> 13;
    h *= 0xc2b2ae35;
    h ^= h >> 16;
    return h;
}

/**
 * @brief Equality function for integer values
 *
 * Compares two integers for equality based on their values.
 *
 * @param[in] a First integer to compare
 * @param[in] b Second integer to compare
 * @return true if both integers have the same value, false otherwise
 */
bool zzIntEquals(const void *a, const void *b) {
    return *(const int*)a == *(const int*)b;
}

/**
 * @brief Hash function for long integer values
 *
 * Computes a hash using a variant of the MurmurHash3 algorithm adapted for 64-bit values.
 * This provides good distribution for long integer values.
 *
 * @param[in] key Pointer to the long integer to hash
 * @return 32-bit hash value derived from the long integer value
 */
uint32_t zzLongHash(const void *key) {
    long val = *(const long*)key;
    uint64_t h = (uint64_t)val;
    h ^= h >> 33;
    h *= 0xff51afd7ed558ccdULL;
    h ^= h >> 33;
    h *= 0xc4ceb9fe1a85ec53ULL;
    h ^= h >> 33;
    return (uint32_t)h;
}

/**
 * @brief Equality function for long integer values
 *
 * Compares two long integers for equality based on their values.
 *
 * @param[in] a First long integer to compare
 * @param[in] b Second long integer to compare
 * @return true if both long integers have the same value, false otherwise
 */
bool zzLongEquals(const void *a, const void *b) {
    return *(const long*)a == *(const long*)b;
}

/* =========================================================
 * Floating Point Hash and Equality Functions
 * ========================================================= */

/**
 * @brief Hash function for float values
 *
 * Computes a hash by directly using the bit representation of the float.
 * This preserves the uniqueness of different float values.
 *
 * @param[in] key Pointer to the float to hash
 * @return 32-bit hash value derived from the float's bit representation
 */
uint32_t zzFloatHash(const void *key) {
    float val = *(const float*)key;
    uint32_t bits;
    memcpy(&bits, &val, sizeof(uint32_t));
    return bits;
}

/**
 * @brief Equality function for float values
 *
 * Compares two floats for equality based on their values.
 * Note: This performs exact equality comparison which may not be suitable
 * for all floating-point comparison scenarios due to precision issues.
 *
 * @param[in] a First float to compare
 * @param[in] b Second float to compare
 * @return true if both floats have the same value, false otherwise
 */
bool zzFloatEquals(const void *a, const void *b) {
    return *(const float*)a == *(const float*)b;
}

/**
 * @brief Hash function for double values
 *
 * Computes a hash by combining the upper and lower 32 bits of the double's
 * bit representation. This preserves the uniqueness of different double values.
 *
 * @param[in] key Pointer to the double to hash
 * @return 32-bit hash value derived from the double's bit representation
 */
uint32_t zzDoubleHash(const void *key) {
    double val = *(const double*)key;
    uint64_t bits;
    memcpy(&bits, &val, sizeof(uint64_t));
    return (uint32_t)(bits ^ (bits >> 32));
}

/**
 * @brief Equality function for double values
 *
 * Compares two doubles for equality based on their values.
 * Note: This performs exact equality comparison which may not be suitable
 * for all floating-point comparison scenarios due to precision issues.
 *
 * @param[in] a First double to compare
 * @param[in] b Second double to compare
 * @return true if both doubles have the same value, false otherwise
 */
bool zzDoubleEquals(const void *a, const void *b) {
    return *(const double*)a == *(const double*)b;
}

/* =========================================================
 * String Hash and Equality Functions
 * ========================================================= */

/**
 * @brief Hash function for string values (pointer to char*)
 *
 * Computes a hash using the FNV-1a algorithm, which provides good distribution
 * for string values. This function expects a pointer to a char* (i.e., the
 * address of a string pointer).
 *
 * @param[in] key Pointer to a pointer to the string to hash
 * @return 32-bit hash value derived from the string content
 */
uint32_t zzStringHash(const void *key) {
    const char *str = *(const char**)key;
    uint32_t hash = 2166136261u;
    while (*str) {
        hash ^= (uint8_t)*str++;
        hash *= 16777619;
    }
    return hash;
}

/**
 * @brief Equality function for string values (pointer to char*)
 *
 * Compares two strings for equality using strcmp. This function expects
 * pointers to char* (i.e., the addresses of string pointers).
 *
 * @param[in] a First string pointer to compare
 * @param[in] b Second string pointer to compare
 * @return true if both strings have the same content, false otherwise
 */
bool zzStringEquals(const void *a, const void *b) {
    return strcmp(*(const char**)a, *(const char**)b) == 0;
}
