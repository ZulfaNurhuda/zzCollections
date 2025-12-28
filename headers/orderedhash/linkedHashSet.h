#ifndef LINKED_HASH_SET_H
#define LINKED_HASH_SET_H

#include "common.h"

typedef struct LHSetNode {
    struct LHSetNode *hashNext;
    struct LHSetNode *prev;
    struct LHSetNode *next;
    uint32_t hash;
    unsigned char key[];
} LHSetNode;

typedef struct {
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

bool zzLinkedHashSetInit(zzLinkedHashSet *lhs, size_t keySize, size_t capacity,
                       zzHashFn hashFn, zzEqualsFn equalsFn, zzFreeFn keyFree);
void zzLinkedHashSetFree(zzLinkedHashSet *lhs);
bool zzLinkedHashSetInsert(zzLinkedHashSet *lhs, const void *key);
bool zzLinkedHashSetContains(const zzLinkedHashSet *lhs, const void *key);
bool zzLinkedHashSetRemove(zzLinkedHashSet *lhs, const void *key);
void zzLinkedHashSetClear(zzLinkedHashSet *lhs);
bool zzLinkedHashSetGetFirst(const zzLinkedHashSet *lhs, void *keyOut);
bool zzLinkedHashSetGetLast(const zzLinkedHashSet *lhs, void *keyOut);

#endif
