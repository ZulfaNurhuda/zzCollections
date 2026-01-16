#include "hashMap.h"
#include <string.h>
#include <stdlib.h>

zzOpResult zzHashMapInit(zzHashMap *hm, size_t keySize, size_t valueSize, size_t capacity, zzHashFn hashFn, zzEqualsFn equalsFn, zzFreeFn keyFree, zzFreeFn valueFree) {
    if (!hm) return ZZ_ERR("HashMap pointer is NULL");
    if (keySize == 0) return ZZ_ERR("Key size cannot be zero");
    if (valueSize == 0) return ZZ_ERR("Value size cannot be zero");
    if (capacity == 0) capacity = 16;

    hm->buckets = calloc(capacity, sizeof(MapNode*));
    if (!hm->buckets) return ZZ_ERR("Failed to allocate buckets");

    hm->keySize = keySize;
    hm->valueSize = valueSize;
    hm->capacity = capacity;
    hm->size = 0;
    hm->loadFactor = 0.75f;
    hm->hashFn = hashFn ? hashFn : zzDefaultHash;
    hm->equalsFn = equalsFn ? equalsFn : zzDefaultEquals;
    hm->keyFree = keyFree;
    hm->valueFree = valueFree;
    return ZZ_OK();
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

static zzOpResult zzHashMapRehash(zzHashMap *hm) {
    size_t newCap = hm->capacity * 2;
    MapNode **newBuckets = calloc(newCap, sizeof(MapNode*));
    if (!newBuckets) return ZZ_ERR("Failed to rehash (out of memory)");

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
    return ZZ_OK();
}

zzOpResult zzHashMapPut(zzHashMap *hm, const void *key, const void *value) {
    if (!hm) return ZZ_ERR("HashMap pointer is NULL");
    if (!key) return ZZ_ERR("Key pointer is NULL");
    if (!value) return ZZ_ERR("Value pointer is NULL");

    uint32_t hash = hm->hashFn(key);
    size_t idx = hash % hm->capacity;

    MapNode *cur = hm->buckets[idx];
    while (cur) {
        if (cur->hash == hash && hm->equalsFn(cur->data, key)) {
            if (hm->valueFree) hm->valueFree(cur->data + hm->keySize);
            memcpy(cur->data + hm->keySize, value, hm->valueSize);
            return ZZ_OK();
        }
        cur = cur->next;
    }

    MapNode *node = malloc(sizeof(MapNode) + hm->keySize + hm->valueSize);
    if (!node) return ZZ_ERR("Failed to allocate node");

    node->hash = hash;
    memcpy(node->data, key, hm->keySize);
    memcpy(node->data + hm->keySize, value, hm->valueSize);
    node->next = hm->buckets[idx];
    hm->buckets[idx] = node;
    hm->size++;

    if ((float)hm->size / hm->capacity > hm->loadFactor) {
        zzOpResult rehashResult = zzHashMapRehash(hm);
        if (ZZ_IS_ERR(rehashResult)) return rehashResult;
    }

    return ZZ_OK();
}

zzOpResult zzHashMapGet(const zzHashMap *hm, const void *key, void *valueOut) {
    if (!hm) return ZZ_ERR("HashMap pointer is NULL");
    if (!key) return ZZ_ERR("Key pointer is NULL");
    if (!valueOut) return ZZ_ERR("Value output buffer is NULL");

    uint32_t hash = hm->hashFn(key);
    size_t idx = hash % hm->capacity;

    MapNode *cur = hm->buckets[idx];
    while (cur) {
        if (cur->hash == hash && hm->equalsFn(cur->data, key)) {
            memcpy(valueOut, cur->data + hm->keySize, hm->valueSize);
            return ZZ_OK();
        }
        cur = cur->next;
    }
    return ZZ_ERR("Key not found");
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

zzOpResult zzHashMapRemove(zzHashMap *hm, const void *key) {
    if (!hm) return ZZ_ERR("HashMap pointer is NULL");
    if (!key) return ZZ_ERR("Key pointer is NULL");

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
            return ZZ_OK();
        }
        cur = &node->next;
    }
    return ZZ_ERR("Key not found");
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