#ifndef LINKED_HASH_SET_H
#define LINKED_HASH_SET_H

#include "types.h"
#include "utils.h"
#include "result.h"

typedef struct LHSetNode {
    struct LHSetNode *hashNext;
    struct LHSetNode *prev;
    struct LHSetNode *next;
    uint32_t hash;
    unsigned char key[];
} LHSetNode;

typedef struct zzLinkedHashSet {
    LHSetNode **buckets;
    LHSetNode *head;
    LHSetNode *tail;
    size_t keySize;
    size_t capacity;
    size_t size;
    float loadFactor;
    zzHashFn hashFn;
    zzEqualsFn equalsFn;
    zzFreeFn keyFree;
} zzLinkedHashSet;

zzOpResult zzLinkedHashSetInit(zzLinkedHashSet *lhs, size_t keySize, size_t capacity, zzHashFn hashFn, zzEqualsFn equalsFn, zzFreeFn keyFree);

void zzLinkedHashSetFree(zzLinkedHashSet *lhs);

zzOpResult zzLinkedHashSetInsert(zzLinkedHashSet *lhs, const void *key);

bool zzLinkedHashSetContains(const zzLinkedHashSet *lhs, const void *key);

zzOpResult zzLinkedHashSetRemove(zzLinkedHashSet *lhs, const void *key);

void zzLinkedHashSetClear(zzLinkedHashSet *lhs);

zzOpResult zzLinkedHashSetGetFirst(const zzLinkedHashSet *lhs, void *keyOut);

zzOpResult zzLinkedHashSetGetLast(const zzLinkedHashSet *lhs, void *keyOut);

#endif