#include "utils.h"
#include <string.h>

uint32_t zzDefaultHash(const void *key) {
    return (uint32_t)(uintptr_t)key;
}

bool zzDefaultEquals(const void *a, const void *b) {
    return a == b;
}

int zzDefaultCompare(const void *a, const void *b) {
    uintptr_t ua = (uintptr_t)a, ub = (uintptr_t)b;
    return (ua > ub) - (ua < ub);
}

uint32_t zzCharHash(const void *key) {
    return (uint32_t)(*(const char*)key);
}

bool zzCharEquals(const void *a, const void *b) {
    return *(const char*)a == *(const char*)b;
}

int zzCharCompare(const void *a, const void *b) {
    char ca = *(const char*)a, cb = *(const char*)b;
    return (ca > cb) - (ca < cb);
}

uint32_t zzIntHash(const void *key) {
    uint32_t h = (uint32_t)*(const int*)key;
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

int zzIntCompare(const void *a, const void *b) {
    int ia = *(const int*)a, ib = *(const int*)b;
    return (ia > ib) - (ia < ib);
}

uint32_t zzLongHash(const void *key) {
    uint64_t h = (uint64_t)*(const long*)key;
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

int zzLongCompare(const void *a, const void *b) {
    long la = *(const long*)a, lb = *(const long*)b;
    return (la > lb) - (la < lb);
}

uint32_t zzFloatHash(const void *key) {
    uint32_t bits;
    memcpy(&bits, key, sizeof(uint32_t));
    return bits;
}

bool zzFloatEquals(const void *a, const void *b) {
    return *(const float*)a == *(const float*)b;
}

int zzFloatCompare(const void *a, const void *b) {
    float fa = *(const float*)a, fb = *(const float*)b;
    return (fa > fb) - (fa < fb);
}

uint32_t zzDoubleHash(const void *key) {
    uint64_t bits;
    memcpy(&bits, key, sizeof(uint64_t));
    return (uint32_t)(bits ^ (bits >> 32));
}

bool zzDoubleEquals(const void *a, const void *b) {
    return *(const double*)a == *(const double*)b;
}

int zzDoubleCompare(const void *a, const void *b) {
    double da = *(const double*)a, db = *(const double*)b;
    return (da > db) - (da < db);
}

uint32_t zzStringHash(const void *key) {
    const char *str = *(const char**)key;
    uint32_t hash = 2166136261u;
    while (*str) {
        hash = (hash ^ (uint8_t)*str++) * 16777619;
    }
    return hash;
}

bool zzStringEquals(const void *a, const void *b) {
    return strcmp(*(const char**)a, *(const char**)b) == 0;
}

int zzStringCompare(const void *a, const void *b) {
    return strcmp(*(const char**)a, *(const char**)b);
}