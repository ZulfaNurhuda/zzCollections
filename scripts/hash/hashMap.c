#include "hashMap.h"
#include "memory.h"
#include <stdlib.h>

bool zzHashMapInit(zzHashMap *hm, size_t keySize, size_t valueSize, size_t capacity,
                 zzHashFn hashFn, zzEqualsFn equalsFn, zzFreeFn keyFree, zzFreeFn valueFree) {
    if (!hm || keySize == 0 || valueSize == 0) return false;
    if (capacity == 0) capacity = 16;
    
    hm->buckets = calloc(capacity, sizeof(MapNode*));
    if (!hm->buckets) return false;
    
    hm->keySize = keySize;
    hm->valueSize = valueSize;
    hm->capacity = capacity;
    hm->size = 0;
    hm->loadFactor = 0.75f;
    hm->hashFn = hashFn ? hashFn : zzDefaultHash;
    hm->equalsFn = equalsFn ? equalsFn : zzDefaultEquals;
    hm->keyFree = keyFree;
    hm->valueFree = valueFree;
    return true;
}

void zzHashMapFree(zzHashMap *hm) {
    if (!hm || !hm->buckets) return;
    
    for (size_t i = 0; i < hm->capacity; i++) {
        MapNode *cur = hm->buckets[i];
        while (cur) {
            MapNode *next = cur->next;
            if (hm->keyFree) hm->keyFree(cur->data);
            if (hm->valueFree) hm->valueFree(cur->data + hm->keySize);
            free(cur);
            cur = next;
        }
    }
    free(hm->buckets);
    hm->buckets = NULL;
    hm->size = 0;
}

static bool zzHashMapRehash(zzHashMap *hm) {
    size_t newCap = hm->capacity * 2;
    MapNode **newBuckets = calloc(newCap, sizeof(MapNode*));
    if (!newBuckets) return false;
    
    for (size_t i = 0; i < hm->capacity; i++) {
        MapNode *cur = hm->buckets[i];
        while (cur) {
            MapNode *next = cur->next;
            size_t idx = cur->hash % newCap;
            cur->next = newBuckets[idx];
            newBuckets[idx] = cur;
            cur = next;
        }
    }
    
    free(hm->buckets);
    hm->buckets = newBuckets;
    hm->capacity = newCap;
    return true;
}

bool zzHashMapPut(zzHashMap *hm, const void *key, const void *value) {
    if (!hm || !key || !value) return false;
    
    uint32_t hash = hm->hashFn(key);
    size_t idx = hash % hm->capacity;
    
    MapNode *cur = hm->buckets[idx];
    while (cur) {
        if (cur->hash == hash && hm->equalsFn(cur->data, key)) {
            if (hm->valueFree) hm->valueFree(cur->data + hm->keySize);
            zzMemoryCopy(cur->data + hm->keySize, value, hm->valueSize);
            return true;
        }
        cur = cur->next;
    }
    
    MapNode *node = malloc(sizeof(MapNode) + hm->keySize + hm->valueSize);
    if (!node) return false;
    
    node->hash = hash;
    zzMemoryCopy(node->data, key, hm->keySize);
    zzMemoryCopy(node->data + hm->keySize, value, hm->valueSize);
    node->next = hm->buckets[idx];
    hm->buckets[idx] = node;
    hm->size++;
    
    if ((float)hm->size / hm->capacity > hm->loadFactor) {
        zzHashMapRehash(hm);
    }
    
    return true;
}

bool zzHashMapGet(const zzHashMap *hm, const void *key, void *valueOut) {
    if (!hm || !key || !valueOut) return false;
    
    uint32_t hash = hm->hashFn(key);
    size_t idx = hash % hm->capacity;
    
    MapNode *cur = hm->buckets[idx];
    while (cur) {
        if (cur->hash == hash && hm->equalsFn(cur->data, key)) {
            zzMemoryCopy(valueOut, cur->data + hm->keySize, hm->valueSize);
            return true;
        }
        cur = cur->next;
    }
    return false;
}

bool zzHashMapContains(const zzHashMap *hm, const void *key) {
    if (!hm || !key) return false;
    
    uint32_t hash = hm->hashFn(key);
    size_t idx = hash % hm->capacity;
    
    MapNode *cur = hm->buckets[idx];
    while (cur) {
        if (cur->hash == hash && hm->equalsFn(cur->data, key))
            return true;
        cur = cur->next;
    }
    return false;
}

bool zzHashMapRemove(zzHashMap *hm, const void *key) {
    if (!hm || !key) return false;
    
    uint32_t hash = hm->hashFn(key);
    size_t idx = hash % hm->capacity;
    
    MapNode **cur = &hm->buckets[idx];
    while (*cur) {
        MapNode *node = *cur;
        if (node->hash == hash && hm->equalsFn(node->data, key)) {
            *cur = node->next;
            if (hm->keyFree) hm->keyFree(node->data);
            if (hm->valueFree) hm->valueFree(node->data + hm->keySize);
            free(node);
            hm->size--;
            return true;
        }
        cur = &node->next;
    }
    return false;
}

void zzHashMapClear(zzHashMap *hm) {
    if (!hm) return;
    
    for (size_t i = 0; i < hm->capacity; i++) {
        MapNode *cur = hm->buckets[i];
        while (cur) {
            MapNode *next = cur->next;
            if (hm->keyFree) hm->keyFree(cur->data);
            if (hm->valueFree) hm->valueFree(cur->data + hm->keySize);
            free(cur);
            cur = next;
        }
        hm->buckets[i] = NULL;
    }
    hm->size = 0;
}
