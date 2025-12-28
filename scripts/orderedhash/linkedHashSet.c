#include "linkedHashSet.h"
#include "memory.h"
#include <stdlib.h>

bool zzLinkedHashSetInit(zzLinkedHashSet *lhs, size_t keySize, size_t capacity,
                       zzHashFn hashFn, zzEqualsFn equalsFn, zzFreeFn keyFree) {
    if (!lhs || keySize == 0) return false;
    if (capacity == 0) capacity = 16;
    
    lhs->buckets = calloc(capacity, sizeof(LHSetNode*));
    if (!lhs->buckets) return false;
    
    lhs->head = NULL;
    lhs->tail = NULL;
    lhs->keySize = keySize;
    lhs->capacity = capacity;
    lhs->size = 0;
    lhs->loadFactor = 0.75f;
    lhs->hashFn = hashFn ? hashFn : zzDefaultHash;
    lhs->equalsFn = equalsFn ? equalsFn : zzDefaultEquals;
    lhs->keyFree = keyFree;
    return true;
}

void zzLinkedHashSetFree(zzLinkedHashSet *lhs) {
    if (!lhs || !lhs->buckets) return;
    
    LHSetNode *cur = lhs->head;
    while (cur) {
        LHSetNode *next = cur->next;
        if (lhs->keyFree) lhs->keyFree(cur->key);
        free(cur);
        cur = next;
    }
    free(lhs->buckets);
    lhs->buckets = NULL;
    lhs->head = lhs->tail = NULL;
    lhs->size = 0;
}

static bool zzLinkedHashSetRehash(zzLinkedHashSet *lhs) {
    size_t newCap = lhs->capacity * 2;
    LHSetNode **newBuckets = calloc(newCap, sizeof(LHSetNode*));
    if (!newBuckets) return false;
    
    LHSetNode *cur = lhs->head;
    while (cur) {
        size_t idx = cur->hash % newCap;
        cur->hashNext = newBuckets[idx];
        newBuckets[idx] = cur;
        cur = cur->next;
    }
    
    free(lhs->buckets);
    lhs->buckets = newBuckets;
    lhs->capacity = newCap;
    return true;
}

bool zzLinkedHashSetInsert(zzLinkedHashSet *lhs, const void *key) {
    if (!lhs || !key) return false;
    
    uint32_t hash = lhs->hashFn(key);
    size_t idx = hash % lhs->capacity;
    
    LHSetNode *cur = lhs->buckets[idx];
    while (cur) {
        if (cur->hash == hash && lhs->equalsFn(cur->key, key))
            return false;
        cur = cur->hashNext;
    }
    
    LHSetNode *node = malloc(sizeof(LHSetNode) + lhs->keySize);
    if (!node) return false;
    
    node->hash = hash;
    zzMemoryCopy(node->key, key, lhs->keySize);
    
    node->hashNext = lhs->buckets[idx];
    lhs->buckets[idx] = node;
    
    node->prev = lhs->tail;
    node->next = NULL;
    if (lhs->tail) lhs->tail->next = node;
    lhs->tail = node;
    if (!lhs->head) lhs->head = node;
    
    lhs->size++;
    
    if ((float)lhs->size / lhs->capacity > lhs->loadFactor) {
        zzLinkedHashSetRehash(lhs);
    }
    
    return true;
}

bool zzLinkedHashSetContains(const zzLinkedHashSet *lhs, const void *key) {
    if (!lhs || !key) return false;
    
    uint32_t hash = lhs->hashFn(key);
    size_t idx = hash % lhs->capacity;
    
    LHSetNode *cur = lhs->buckets[idx];
    while (cur) {
        if (cur->hash == hash && lhs->equalsFn(cur->key, key))
            return true;
        cur = cur->hashNext;
    }
    return false;
}

bool zzLinkedHashSetRemove(zzLinkedHashSet *lhs, const void *key) {
    if (!lhs || !key) return false;
    
    uint32_t hash = lhs->hashFn(key);
    size_t idx = hash % lhs->capacity;
    
    LHSetNode **hashCur = &lhs->buckets[idx];
    while (*hashCur) {
        LHSetNode *node = *hashCur;
        if (node->hash == hash && lhs->equalsFn(node->key, key)) {
            *hashCur = node->hashNext;
            
            if (node->prev) node->prev->next = node->next;
            else lhs->head = node->next;
            
            if (node->next) node->next->prev = node->prev;
            else lhs->tail = node->prev;
            
            if (lhs->keyFree) lhs->keyFree(node->key);
            free(node);
            lhs->size--;
            return true;
        }
        hashCur = &node->hashNext;
    }
    return false;
}

void zzLinkedHashSetClear(zzLinkedHashSet *lhs) {
    if (!lhs) return;
    
    LHSetNode *cur = lhs->head;
    while (cur) {
        LHSetNode *next = cur->next;
        if (lhs->keyFree) lhs->keyFree(cur->key);
        free(cur);
        cur = next;
    }
    
    for (size_t i = 0; i < lhs->capacity; i++) {
        lhs->buckets[i] = NULL;
    }
    
    lhs->head = lhs->tail = NULL;
    lhs->size = 0;
}

bool zzLinkedHashSetGetFirst(const zzLinkedHashSet *lhs, void *keyOut) {
    if (!lhs || !lhs->head || !keyOut) return false;
    zzMemoryCopy(keyOut, lhs->head->key, lhs->keySize);
    return true;
}

bool zzLinkedHashSetGetLast(const zzLinkedHashSet *lhs, void *keyOut) {
    if (!lhs || !lhs->tail || !keyOut) return false;
    zzMemoryCopy(keyOut, lhs->tail->key, lhs->keySize);
    return true;
}
