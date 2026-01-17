#ifndef LINKED_HASH_MAP_H
#define LINKED_HASH_MAP_H

#include "types.h"
#include "utils.h"
#include "result.h"

typedef struct LHMapNode {
    struct LHMapNode *hashNext;
    struct LHMapNode *prev;
    struct LHMapNode *next;
    uint32_t hash;
    unsigned char data[];
} LHMapNode;

typedef struct zzLinkedHashMap {
    LHMapNode **buckets;
    LHMapNode *head;
    LHMapNode *tail;
    size_t capacity;
    size_t size;
    size_t keySize;
    size_t valueSize;
    float loadFactor;
    zzHashFn hashFn;
    zzEqualsFn equalsFn;
    zzFreeFn keyFree;
    zzFreeFn valueFree;
} zzLinkedHashMap;

zzOpResult zzLinkedHashMapInit(zzLinkedHashMap *lhm, size_t keySize, size_t valueSize, size_t capacity, zzHashFn hashFn, zzEqualsFn equalsFn, zzFreeFn keyFree, zzFreeFn valueFree);

void zzLinkedHashMapFree(zzLinkedHashMap *lhm);

zzOpResult zzLinkedHashMapPut(zzLinkedHashMap *lhm, const void *key, const void *value);

zzOpResult zzLinkedHashMapGet(const zzLinkedHashMap *lhm, const void *key, void *valueOut);

bool zzLinkedHashMapContains(const zzLinkedHashMap *lhm, const void *key);

zzOpResult zzLinkedHashMapRemove(zzLinkedHashMap *lhm, const void *key);

void zzLinkedHashMapClear(zzLinkedHashMap *lhm);

zzOpResult zzLinkedHashMapGetFirst(const zzLinkedHashMap *lhm, void *keyOut, void *valueOut);

zzOpResult zzLinkedHashMapGetLast(const zzLinkedHashMap *lhm, void *keyOut, void *valueOut);

#endif