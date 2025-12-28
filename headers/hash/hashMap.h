#ifndef HASH_MAP_H
#define HASH_MAP_H

#include "common.h"

typedef struct MapNode {
    struct MapNode *next;
    uint32_t hash;
    unsigned char data[];
} MapNode;

typedef struct {
    MapNode **buckets;
    size_t keySize;
    size_t valueSize;
    size_t capacity;
    size_t size;
    float loadFactor;
    zzHashFn hashFn;
    zzEqualsFn equalsFn;
    zzFreeFn keyFree;
    zzFreeFn valueFree;
} zzHashMap;

bool zzHashMapInit(zzHashMap *hm, size_t keySize, size_t valueSize, size_t capacity,
                 zzHashFn hashFn, zzEqualsFn equalsFn, zzFreeFn keyFree, zzFreeFn valueFree);
void zzHashMapFree(zzHashMap *hm);
bool zzHashMapPut(zzHashMap *hm, const void *key, const void *value);
bool zzHashMapGet(const zzHashMap *hm, const void *key, void *valueOut);
bool zzHashMapContains(const zzHashMap *hm, const void *key);
bool zzHashMapRemove(zzHashMap *hm, const void *key);
void zzHashMapClear(zzHashMap *hm);

#endif
