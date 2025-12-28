#ifndef HASH_MAP_H
#define HASH_MAP_H

#include "common.h"
#include "result.h"

typedef struct MapNode {
    struct MapNode *next;
    uint32_t hash;
    unsigned char data[];
} MapNode;

/** @brief Hash map with separate chaining */
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

/** @brief Initialize HashMap */
zzOpResult zzHashMapInit(zzHashMap *hm, size_t keySize, size_t valueSize, size_t capacity,
                 zzHashFn hashFn, zzEqualsFn equalsFn, zzFreeFn keyFree, zzFreeFn valueFree);

/** @brief Free HashMap */
void zzHashMapFree(zzHashMap *hm);

/** @brief Put key-value pair */
zzOpResult zzHashMapPut(zzHashMap *hm, const void *key, const void *value);

/** @brief Get value by key */
zzOpResult zzHashMapGet(const zzHashMap *hm, const void *key, void *valueOut);

/** @brief Check if key exists */
bool zzHashMapContains(const zzHashMap *hm, const void *key);

/** @brief Remove key-value pair */
zzOpResult zzHashMapRemove(zzHashMap *hm, const void *key);

/** @brief Clear all entries */
void zzHashMapClear(zzHashMap *hm);

#endif
