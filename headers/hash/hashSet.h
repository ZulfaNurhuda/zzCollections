#ifndef HASH_SET_H
#define HASH_SET_H

#include "types.h"
#include "utils.h"
#include "result.h"

typedef struct SetNode {
    struct SetNode *next;
    uint32_t hash;
    unsigned char key[];
} SetNode;

typedef struct zzHashSet {
    SetNode **buckets;
    size_t capacity;
    size_t size;
    size_t keySize;
    float loadFactor;
    zzHashFn hashFn;
    zzEqualsFn equalsFn;
    zzFreeFn keyFree;
} zzHashSet;

zzOpResult zzHashSetInit(zzHashSet *s, size_t keySize, size_t capacity, zzHashFn hashFn, zzEqualsFn equalsFn, zzFreeFn keyFree);

void zzHashSetFree(zzHashSet *s);

zzOpResult zzHashSetInsert(zzHashSet *s, const void *key);

bool zzHashSetContains(const zzHashSet *s, const void *key);

zzOpResult zzHashSetRemove(zzHashSet *s, const void *key);

void zzHashSetClear(zzHashSet *s);

void zzHashSetForEach(zzHashSet *s, zzForEachFn fn, void *userdata);

zzOpResult zzHashSetUnion(zzHashSet *s1, const zzHashSet *s2);

zzOpResult zzHashSetIntersection(const zzHashSet *s1, const zzHashSet *s2, zzHashSet *result, zzHashFn hashFn, zzEqualsFn equalsFn, zzFreeFn keyFree);

zzOpResult zzHashSetDifference(const zzHashSet *s1, const zzHashSet *s2, zzHashSet *result, zzHashFn hashFn, zzEqualsFn equalsFn, zzFreeFn keyFree);

#endif