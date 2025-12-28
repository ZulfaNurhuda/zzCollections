#ifndef LINKED_HASH_MAP_H
#define LINKED_HASH_MAP_H

#include "common.h"

typedef struct LHMapNode {
    struct LHMapNode *hashNext;
    struct LHMapNode *prev;
    struct LHMapNode *next;
    uint32_t hash;
    unsigned char data[];
} LHMapNode;

typedef struct {
    LHMapNode **buckets;
    LHMapNode *head;
    LHMapNode *tail;
    size_t keySize;
    size_t valueSize;
    size_t capacity;
    size_t size;
    float loadFactor;
    zzHashFn hashFn;
    zzEqualsFn equalsFn;
    zzFreeFn keyFree;
    zzFreeFn valueFree;
} zzLinkedHashMap;

bool zzLinkedHashMapInit(zzLinkedHashMap *lhm, size_t keySize, size_t valueSize, size_t capacity,
                       zzHashFn hashFn, zzEqualsFn equalsFn, zzFreeFn keyFree, zzFreeFn valueFree);
void zzLinkedHashMapFree(zzLinkedHashMap *lhm);
bool zzLinkedHashMapPut(zzLinkedHashMap *lhm, const void *key, const void *value);
bool zzLinkedHashMapGet(const zzLinkedHashMap *lhm, const void *key, void *valueOut);
bool zzLinkedHashMapContains(const zzLinkedHashMap *lhm, const void *key);
bool zzLinkedHashMapRemove(zzLinkedHashMap *lhm, const void *key);
void zzLinkedHashMapClear(zzLinkedHashMap *lhm);
bool zzLinkedHashMapGetFirst(const zzLinkedHashMap *lhm, void *keyOut, void *valueOut);
bool zzLinkedHashMapGetLast(const zzLinkedHashMap *lhm, void *keyOut, void *valueOut);

#endif
