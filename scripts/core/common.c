#include "common.h"
#include <string.h>

uint32_t zzDefaultHash(const void *key) {
    return (uint32_t)(uintptr_t)key;
}

bool zzDefaultEquals(const void *a, const void *b) {
    return a == b;
}

uint32_t zzIntHash(const void *key) {
    int val = *(const int*)key;
    // Simple but effective integer hash
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
