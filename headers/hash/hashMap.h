#ifndef HASH_MAP_H
#define HASH_MAP_H

#include "types.h"
#include "utils.h"
#include "result.h"

typedef struct MapNode {
    struct MapNode *next;
    uint32_t hash;
    unsigned char data[];
} MapNode;

typedef struct zzHashMap {
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

zzOpResult zzHashMapInit(zzHashMap *hm, size_t keySize, size_t valueSize, size_t capacity, zzHashFn hashFn, zzEqualsFn equalsFn, zzFreeFn keyFree, zzFreeFn valueFree);

void zzHashMapFree(zzHashMap *hm);

zzOpResult zzHashMapPut(zzHashMap *hm, const void *key, const void *value);

zzOpResult zzHashMapGet(const zzHashMap *hm, const void *key, void *valueOut);

bool zzHashMapContains(const zzHashMap *hm, const void *key);

zzOpResult zzHashMapRemove(zzHashMap *hm, const void *key);

void zzHashMapClear(zzHashMap *hm);

#endif