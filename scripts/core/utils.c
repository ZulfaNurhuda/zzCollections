#include "utils.h"
#include <string.h>

/* =========================================================
 * Default Hash and Equality Functions
 * ========================================================= */

uint32_t zzDefaultHash(const void *key) {
    return (uint32_t)(uintptr_t)key;
}

bool zzDefaultEquals(const void *a, const void *b) {
    return a == b;
}

/* =========================================================
 * Integer Type Hash and Equality Functions
 * ========================================================= */

uint32_t zzCharHash(const void *key) {
    return (uint32_t)(*(const char*)key);
}

bool zzCharEquals(const void *a, const void *b) {
    return *(const char*)a == *(const char*)b;
}

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

bool zzIntEquals(const void *a, const void *b) {
    return *(const int*)a == *(const int*)b;
}

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

bool zzLongEquals(const void *a, const void *b) {
    return *(const long*)a == *(const long*)b;
}

/* =========================================================
 * Floating Point Hash and Equality Functions
 * ========================================================= */

uint32_t zzFloatHash(const void *key) {
    float val = *(const float*)key;
    uint32_t bits;
    memcpy(&bits, &val, sizeof(uint32_t));
    return bits;
}

bool zzFloatEquals(const void *a, const void *b) {
    return *(const float*)a == *(const float*)b;
}

uint32_t zzDoubleHash(const void *key) {
    double val = *(const double*)key;
    uint64_t bits;
    memcpy(&bits, &val, sizeof(uint64_t));
    return (uint32_t)(bits ^ (bits >> 32));
}

bool zzDoubleEquals(const void *a, const void *b) {
    return *(const double*)a == *(const double*)b;
}

/* =========================================================
 * String Hash and Equality Functions
 * ========================================================= */

uint32_t zzStringHash(const void *key) {
    const char *str = *(const char**)key;
    uint32_t hash = 2166136261u;
    while (*str) {
        hash ^= (uint8_t)*str++;
        hash *= 16777619;
    }
    return hash;
}

bool zzStringEquals(const void *a, const void *b) {
    return strcmp(*(const char**)a, *(const char**)b) == 0;
}
