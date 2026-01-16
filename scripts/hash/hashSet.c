#include "hashSet.h"
#include <string.h>
#include <stdlib.h>

zzOpResult zzHashSetInit(zzHashSet *s, size_t keySize, size_t capacity, zzHashFn hashFn, zzEqualsFn equalsFn, zzFreeFn keyFree) {
    if (!s) return ZZ_ERR("HashSet pointer is NULL");
    if (keySize == 0) return ZZ_ERR("Key size cannot be zero");
    if (capacity == 0) capacity = 16;

    s->buckets = calloc(capacity, sizeof(SetNode*));
    if (!s->buckets) return ZZ_ERR("Failed to allocate buckets");

    s->keySize = keySize;
    s->capacity = capacity;
    s->size = 0;
    s->loadFactor = 0.75f;
    s->hashFn = hashFn ? hashFn : zzDefaultHash;
    s->equalsFn = equalsFn ? equalsFn : zzDefaultEquals;
    s->keyFree = keyFree;
    return ZZ_OK();
}

void zzHashSetFree(zzHashSet *s) {
    if (!s || !s->buckets) return;

    for (size_t i = 0; i < s->capacity; i++) {
        SetNode *cur = s->buckets[i];
        while (cur) {
            SetNode *next = cur->next;
            if (s->keyFree) s->keyFree(cur->key);
            free(cur);
            cur = next;
        }
    }
    free(s->buckets);
    s->buckets = NULL;
    s->size = 0;
}

static zzOpResult zzHashSetRehash(zzHashSet *s) {
    size_t newCap = s->capacity * 2;
    SetNode **newBuckets = calloc(newCap, sizeof(SetNode*));
    if (!newBuckets) return ZZ_ERR("Failed to rehash (out of memory)");

    for (size_t i = 0; i < s->capacity; i++) {
        SetNode *cur = s->buckets[i];
        while (cur) {
            SetNode *next = cur->next;
            size_t idx = cur->hash % newCap;
            cur->next = newBuckets[idx];
            newBuckets[idx] = cur;
            cur = next;
        }
    }

    free(s->buckets);
    s->buckets = newBuckets;
    s->capacity = newCap;
    return ZZ_OK();
}

zzOpResult zzHashSetInsert(zzHashSet *s, const void *key) {
    if (!s) return ZZ_ERR("HashSet pointer is NULL");
    if (!key) return ZZ_ERR("Key pointer is NULL");

    uint32_t hash = s->hashFn(key);
    size_t idx = hash % s->capacity;

    SetNode *cur = s->buckets[idx];
    while (cur) {
        if (cur->hash == hash && s->equalsFn(cur->key, key))
            return ZZ_ERR("Key already exists");
        cur = cur->next;
    }

    SetNode *node = malloc(sizeof(SetNode) + s->keySize);
    if (!node) return ZZ_ERR("Failed to allocate node");

    node->hash = hash;
    memcpy(node->key, key, s->keySize);
    node->next = s->buckets[idx];
    s->buckets[idx] = node;
    s->size++;

    if ((float)s->size / s->capacity > s->loadFactor) {
        zzOpResult rehashResult = zzHashSetRehash(s);
        if (ZZ_IS_ERR(rehashResult)) return rehashResult;
    }

    return ZZ_OK();
}

bool zzHashSetContains(const zzHashSet *s, const void *key) {
    if (!s || !key) return false;

    uint32_t hash = s->hashFn(key);
    size_t idx = hash % s->capacity;

    SetNode *cur = s->buckets[idx];
    while (cur) {
        if (cur->hash == hash && s->equalsFn(cur->key, key))
            return true;
        cur = cur->next;
    }
    return false;
}

zzOpResult zzHashSetRemove(zzHashSet *s, const void *key) {
    if (!s) return ZZ_ERR("HashSet pointer is NULL");
    if (!key) return ZZ_ERR("Key pointer is NULL");

    uint32_t hash = s->hashFn(key);
    size_t idx = hash % s->capacity;

    SetNode **cur = &s->buckets[idx];
    while (*cur) {
        SetNode *node = *cur;
        if (node->hash == hash && s->equalsFn(node->key, key)) {
            *cur = node->next;
            if (s->keyFree) s->keyFree(node->key);
            free(node);
            s->size--;
            return ZZ_OK();
        }
        cur = &node->next;
    }
    return ZZ_ERR("Key not found");
}

void zzHashSetClear(zzHashSet *s) {
    if (!s) return;

    for (size_t i = 0; i < s->capacity; i++) {
        SetNode *cur = s->buckets[i];
        while (cur) {
            SetNode *next = cur->next;
            if (s->keyFree) s->keyFree(cur->key);
            free(cur);
            cur = next;
        }
        s->buckets[i] = NULL;
    }
    s->size = 0;
}

void zzHashSetForEach(zzHashSet *s, zzForEachFn fn, void *userdata) {
    if (!s || !fn) return;

    for (size_t i = 0; i < s->capacity; i++) {
        SetNode *cur = s->buckets[i];
        while (cur) {
            fn(cur->key, userdata);
            cur = cur->next;
        }
    }
}

zzOpResult zzHashSetUnion(zzHashSet *s1, const zzHashSet *s2) {
    if (!s1) return ZZ_ERR("HashSet s1 pointer is NULL");
    if (!s2) return ZZ_ERR("HashSet s2 pointer is NULL");

    for (size_t i = 0; i < s2->capacity; i++) {
        SetNode *cur = s2->buckets[i];
        while (cur) {
            zzOpResult insertResult = zzHashSetInsert(s1, cur->key);
            if (ZZ_IS_ERR(insertResult) && insertResult.error != NULL) {
                if (insertResult.error[0] != 'K') { 
                    return insertResult;
                }
            }
            cur = cur->next;
        }
    }
    return ZZ_OK();
}

zzOpResult zzHashSetIntersection(const zzHashSet *s1, const zzHashSet *s2, zzHashSet *result, zzHashFn hashFn, zzEqualsFn equalsFn, zzFreeFn keyFree) {
    if (!s1) return ZZ_ERR("HashSet s1 pointer is NULL");
    if (!s2) return ZZ_ERR("HashSet s2 pointer is NULL");
    if (!result) return ZZ_ERR("Result HashSet pointer is NULL");

    zzOpResult initResult = zzHashSetInit(result, s1->keySize, 16, hashFn, equalsFn, keyFree);
    if (ZZ_IS_ERR(initResult)) return initResult;

    for (size_t i = 0; i < s1->capacity; i++) {
        SetNode *cur = s1->buckets[i];
        while (cur) {
            if (zzHashSetContains(s2, cur->key)) {
                zzOpResult insertResult = zzHashSetInsert(result, cur->key);
                if (ZZ_IS_ERR(insertResult)) {
                    zzHashSetFree(result);
                    return insertResult;
                }
            }
            cur = cur->next;
        }
    }

    return ZZ_OK();
}

zzOpResult zzHashSetDifference(const zzHashSet *s1, const zzHashSet *s2, zzHashSet *result, zzHashFn hashFn, zzEqualsFn equalsFn, zzFreeFn keyFree) {
    if (!s1) return ZZ_ERR("HashSet s1 pointer is NULL");
    if (!s2) return ZZ_ERR("HashSet s2 pointer is NULL");
    if (!result) return ZZ_ERR("Result HashSet pointer is NULL");

    zzOpResult initResult = zzHashSetInit(result, s1->keySize, 16, hashFn, equalsFn, keyFree);
    if (ZZ_IS_ERR(initResult)) return initResult;

    for (size_t i = 0; i < s1->capacity; i++) {
        SetNode *cur = s1->buckets[i];
        while (cur) {
            if (!zzHashSetContains(s2, cur->key)) {
                zzOpResult insertResult = zzHashSetInsert(result, cur->key);
                if (ZZ_IS_ERR(insertResult)) {
                    zzHashSetFree(result);
                    return insertResult;
                }
            }
            cur = cur->next;
        }
    }

    return ZZ_OK();
}