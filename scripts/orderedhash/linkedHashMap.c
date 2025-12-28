#include "linkedHashMap.h"
#include "memory.h"
#include <stdlib.h>

bool zzLinkedHashMapInit(zzLinkedHashMap *lhm, size_t keySize, size_t valueSize, size_t capacity,
                       zzHashFn hashFn, zzEqualsFn equalsFn, zzFreeFn keyFree, zzFreeFn valueFree) {
    if (!lhm || keySize == 0 || valueSize == 0) return false;
    if (capacity == 0) capacity = 16;
    
    lhm->buckets = calloc(capacity, sizeof(LHMapNode*));
    if (!lhm->buckets) return false;
    
    lhm->head = NULL;
    lhm->tail = NULL;
    lhm->keySize = keySize;
    lhm->valueSize = valueSize;
    lhm->capacity = capacity;
    lhm->size = 0;
    lhm->loadFactor = 0.75f;
    lhm->hashFn = hashFn ? hashFn : zzDefaultHash;
    lhm->equalsFn = equalsFn ? equalsFn : zzDefaultEquals;
    lhm->keyFree = keyFree;
    lhm->valueFree = valueFree;
    return true;
}

void zzLinkedHashMapFree(zzLinkedHashMap *lhm) {
    if (!lhm || !lhm->buckets) return;
    
    LHMapNode *cur = lhm->head;
    while (cur) {
        LHMapNode *next = cur->next;
        if (lhm->keyFree) lhm->keyFree(cur->data);
        if (lhm->valueFree) lhm->valueFree(cur->data + lhm->keySize);
        free(cur);
        cur = next;
    }
    free(lhm->buckets);
    lhm->buckets = NULL;
    lhm->head = lhm->tail = NULL;
    lhm->size = 0;
}

static bool zzLinkedHashMapRehash(zzLinkedHashMap *lhm) {
    size_t newCap = lhm->capacity * 2;
    LHMapNode **newBuckets = calloc(newCap, sizeof(LHMapNode*));
    if (!newBuckets) return false;
    
    LHMapNode *cur = lhm->head;
    while (cur) {
        size_t idx = cur->hash % newCap;
        cur->hashNext = newBuckets[idx];
        newBuckets[idx] = cur;
        cur = cur->next;
    }
    
    free(lhm->buckets);
    lhm->buckets = newBuckets;
    lhm->capacity = newCap;
    return true;
}

bool zzLinkedHashMapPut(zzLinkedHashMap *lhm, const void *key, const void *value) {
    if (!lhm || !key || !value) return false;
    
    uint32_t hash = lhm->hashFn(key);
    size_t idx = hash % lhm->capacity;
    
    LHMapNode *cur = lhm->buckets[idx];
    while (cur) {
        if (cur->hash == hash && lhm->equalsFn(cur->data, key)) {
            if (lhm->valueFree) lhm->valueFree(cur->data + lhm->keySize);
            zzMemoryCopy(cur->data + lhm->keySize, value, lhm->valueSize);
            return true;
        }
        cur = cur->hashNext;
    }
    
    LHMapNode *node = malloc(sizeof(LHMapNode) + lhm->keySize + lhm->valueSize);
    if (!node) return false;
    
    node->hash = hash;
    zzMemoryCopy(node->data, key, lhm->keySize);
    zzMemoryCopy(node->data + lhm->keySize, value, lhm->valueSize);
    
    node->hashNext = lhm->buckets[idx];
    lhm->buckets[idx] = node;
    
    node->prev = lhm->tail;
    node->next = NULL;
    if (lhm->tail) lhm->tail->next = node;
    lhm->tail = node;
    if (!lhm->head) lhm->head = node;
    
    lhm->size++;
    
    if ((float)lhm->size / lhm->capacity > lhm->loadFactor) {
        zzLinkedHashMapRehash(lhm);
    }
    
    return true;
}

bool zzLinkedHashMapGet(const zzLinkedHashMap *lhm, const void *key, void *valueOut) {
    if (!lhm || !key || !valueOut) return false;
    
    uint32_t hash = lhm->hashFn(key);
    size_t idx = hash % lhm->capacity;
    
    LHMapNode *cur = lhm->buckets[idx];
    while (cur) {
        if (cur->hash == hash && lhm->equalsFn(cur->data, key)) {
            zzMemoryCopy(valueOut, cur->data + lhm->keySize, lhm->valueSize);
            return true;
        }
        cur = cur->hashNext;
    }
    return false;
}

bool zzLinkedHashMapContains(const zzLinkedHashMap *lhm, const void *key) {
    if (!lhm || !key) return false;
    
    uint32_t hash = lhm->hashFn(key);
    size_t idx = hash % lhm->capacity;
    
    LHMapNode *cur = lhm->buckets[idx];
    while (cur) {
        if (cur->hash == hash && lhm->equalsFn(cur->data, key))
            return true;
        cur = cur->hashNext;
    }
    return false;
}

bool zzLinkedHashMapRemove(zzLinkedHashMap *lhm, const void *key) {
    if (!lhm || !key) return false;
    
    uint32_t hash = lhm->hashFn(key);
    size_t idx = hash % lhm->capacity;
    
    LHMapNode **hashCur = &lhm->buckets[idx];
    while (*hashCur) {
        LHMapNode *node = *hashCur;
        if (node->hash == hash && lhm->equalsFn(node->data, key)) {
            *hashCur = node->hashNext;
            
            if (node->prev) node->prev->next = node->next;
            else lhm->head = node->next;
            
            if (node->next) node->next->prev = node->prev;
            else lhm->tail = node->prev;
            
            if (lhm->keyFree) lhm->keyFree(node->data);
            if (lhm->valueFree) lhm->valueFree(node->data + lhm->keySize);
            free(node);
            lhm->size--;
            return true;
        }
        hashCur = &node->hashNext;
    }
    return false;
}

void zzLinkedHashMapClear(zzLinkedHashMap *lhm) {
    if (!lhm) return;
    
    LHMapNode *cur = lhm->head;
    while (cur) {
        LHMapNode *next = cur->next;
        if (lhm->keyFree) lhm->keyFree(cur->data);
        if (lhm->valueFree) lhm->valueFree(cur->data + lhm->keySize);
        free(cur);
        cur = next;
    }
    
    for (size_t i = 0; i < lhm->capacity; i++) {
        lhm->buckets[i] = NULL;
    }
    
    lhm->head = lhm->tail = NULL;
    lhm->size = 0;
}

bool zzLinkedHashMapGetFirst(const zzLinkedHashMap *lhm, void *keyOut, void *valueOut) {
    if (!lhm || !lhm->head) return false;
    if (keyOut) zzMemoryCopy(keyOut, lhm->head->data, lhm->keySize);
    if (valueOut) zzMemoryCopy(valueOut, lhm->head->data + lhm->keySize, lhm->valueSize);
    return true;
}

bool zzLinkedHashMapGetLast(const zzLinkedHashMap *lhm, void *keyOut, void *valueOut) {
    if (!lhm || !lhm->tail) return false;
    if (keyOut) zzMemoryCopy(keyOut, lhm->tail->data, lhm->keySize);
    if (valueOut) zzMemoryCopy(valueOut, lhm->tail->data + lhm->keySize, lhm->valueSize);
    return true;
}
