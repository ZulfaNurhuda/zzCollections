#include "hashSet.h"
#include "memory.h"
#include <stdlib.h>

bool setInit(zzHashSet *s, size_t keySize, size_t capacity,
             zzHashFn hashFn, zzEqualsFn equalsFn, zzFreeFn keyFree) {
    if (!s || keySize == 0) return false;
    if (capacity == 0) capacity = 16;
    
    s->buckets = calloc(capacity, sizeof(SetNode*));
    if (!s->buckets) return false;
    
    s->keySize = keySize;
    s->capacity = capacity;
    s->size = 0;
    s->loadFactor = 0.75f;
    s->hashFn = hashFn ? hashFn : zzDefaultHash;
    s->equalsFn = equalsFn ? equalsFn : zzDefaultEquals;
    s->keyFree = keyFree;
    return true;
}

void setFree(zzHashSet *s) {
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

static bool setRehash(zzHashSet *s) {
    size_t newCap = s->capacity * 2;
    SetNode **newBuckets = calloc(newCap, sizeof(SetNode*));
    if (!newBuckets) return false;
    
    // Rehash all nodes
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
    return true;
}

bool setInsert(zzHashSet *s, const void *key) {
    if (!s || !key) return false;
    
    uint32_t hash = s->hashFn(key);
    size_t idx = hash % s->capacity;
    
    // Check if already exists
    SetNode *cur = s->buckets[idx];
    while (cur) {
        if (cur->hash == hash && s->equalsFn(cur->key, key))
            return false; // Already exists
        cur = cur->next;
    }
    
    // Insert new node
    SetNode *node = malloc(sizeof(SetNode) + s->keySize);
    if (!node) return false;
    
    node->hash = hash;
    zzMemoryCopy(node->key, key, s->keySize);
    node->next = s->buckets[idx];
    s->buckets[idx] = node;
    s->size++;
    
    // Rehash if needed
    if ((float)s->size / s->capacity > s->loadFactor) {
        setRehash(s);
    }
    
    return true;
}

bool setContains(const zzHashSet *s, const void *key) {
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

bool setRemove(zzHashSet *s, const void *key) {
    if (!s || !key) return false;
    
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
            return true;
        }
        cur = &node->next;
    }
    return false;
}

void setClear(zzHashSet *s) {
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

void setForEach(zzHashSet *s, zzForEachFn fn, void *userdata) {
    if (!s || !fn) return;
    
    for (size_t i = 0; i < s->capacity; i++) {
        SetNode *cur = s->buckets[i];
        while (cur) {
            fn(cur->key, userdata);
            cur = cur->next;
        }
    }
}

bool setUnion(zzHashSet *s1, const zzHashSet *s2) {
    if (!s1 || !s2) return false;
    
    for (size_t i = 0; i < s2->capacity; i++) {
        SetNode *cur = s2->buckets[i];
        while (cur) {
            setInsert(s1, cur->key);
            cur = cur->next;
        }
    }
    return true;
}

bool setIntersection(const zzHashSet *s1, const zzHashSet *s2, zzHashSet *result,
                     zzHashFn hashFn, zzEqualsFn equalsFn, zzFreeFn keyFree) {
    if (!s1 || !s2 || !result) return false;
    
    if (!setInit(result, s1->keySize, 16, hashFn, equalsFn, keyFree)) {
        return false;
    }
    
    for (size_t i = 0; i < s1->capacity; i++) {
        SetNode *cur = s1->buckets[i];
        while (cur) {
            if (setContains(s2, cur->key)) {
                if (!setInsert(result, cur->key)) {
                    setFree(result);
                    return false;
                }
            }
            cur = cur->next;
        }
    }
    
    return true;
}

bool setDifference(const zzHashSet *s1, const zzHashSet *s2, zzHashSet *result,
                   zzHashFn hashFn, zzEqualsFn equalsFn, zzFreeFn keyFree) {
    if (!s1 || !s2 || !result) return false;
    
    if (!setInit(result, s1->keySize, 16, hashFn, equalsFn, keyFree)) {
        return false;
    }
    
    for (size_t i = 0; i < s1->capacity; i++) {
        SetNode *cur = s1->buckets[i];
        while (cur) {
            if (!setContains(s2, cur->key)) {
                if (!setInsert(result, cur->key)) {
                    setFree(result);
                    return false;
                }
            }
            cur = cur->next;
        }
    }
    
    return true;
}
