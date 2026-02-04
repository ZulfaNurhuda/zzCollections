#include "linkedHashMap.h"
#include <string.h>
#include <stdlib.h>

zzOpResult zzLinkedHashMapInit(zzLinkedHashMap *lhm, size_t keySize, size_t valueSize, size_t capacity, zzHashFn hashFn, zzEqualsFn equalsFn, zzFreeFn keyFree, zzFreeFn valueFree) {
    if (!lhm) return ZZ_ERR("LinkedHashMap pointer is NULL");
    if (keySize == 0) return ZZ_ERR("Key size cannot be zero");
    if (valueSize == 0) return ZZ_ERR("Value size cannot be zero");
    if (capacity == 0) capacity = 16;

    lhm->buckets = calloc(capacity, sizeof(LHMapNode*));
    if (!lhm->buckets) return ZZ_ERR("Failed to allocate buckets");

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
    return ZZ_OK();
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

static zzOpResult zzLinkedHashMapRehash(zzLinkedHashMap *lhm) {
    size_t newCap = lhm->capacity * 2;
    LHMapNode **newBuckets = calloc(newCap, sizeof(LHMapNode*));
    if (!newBuckets) return ZZ_ERR("Failed to rehash (out of memory)");

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
    return ZZ_OK();
}

zzOpResult zzLinkedHashMapPut(zzLinkedHashMap *lhm, const void *key, const void *value) {
    if (!lhm) return ZZ_ERR("LinkedHashMap pointer is NULL");
    if (!key) return ZZ_ERR("Key pointer is NULL");
    if (!value) return ZZ_ERR("Value pointer is NULL");

    uint32_t hash = lhm->hashFn(key);
    size_t idx = hash % lhm->capacity;

    LHMapNode *cur = lhm->buckets[idx];
    while (cur) {
        if (cur->hash == hash && lhm->equalsFn(cur->data, key)) {
            if (lhm->valueFree) lhm->valueFree(cur->data + lhm->keySize);
            memcpy(cur->data + lhm->keySize, value, lhm->valueSize);
            return ZZ_OK();
        }
        cur = cur->hashNext;
    }

    LHMapNode *node = malloc(sizeof(LHMapNode) + lhm->keySize + lhm->valueSize);
    if (!node) return ZZ_ERR("Failed to allocate node");

    node->hash = hash;
    memcpy(node->data, key, lhm->keySize);
    memcpy(node->data + lhm->keySize, value, lhm->valueSize);

    node->hashNext = lhm->buckets[idx];
    lhm->buckets[idx] = node;

    node->prev = lhm->tail;
    node->next = NULL;
    if (lhm->tail) lhm->tail->next = node;
    lhm->tail = node;
    if (!lhm->head) lhm->head = node;

    lhm->size++;

    if ((float)lhm->size / lhm->capacity > lhm->loadFactor) {
        zzOpResult rehashResult = zzLinkedHashMapRehash(lhm);
        if (ZZ_IS_ERR(rehashResult)) return rehashResult;
    }

    return ZZ_OK();
}

zzOpResult zzLinkedHashMapGet(const zzLinkedHashMap *lhm, const void *key, void *valueOut) {
    if (!lhm) return ZZ_ERR("LinkedHashMap pointer is NULL");
    if (!key) return ZZ_ERR("Key pointer is NULL");
    if (!valueOut) return ZZ_ERR("Value output pointer is NULL");

    uint32_t hash = lhm->hashFn(key);
    size_t idx = hash % lhm->capacity;

    LHMapNode *cur = lhm->buckets[idx];
    while (cur) {
        if (cur->hash == hash && lhm->equalsFn(cur->data, key)) {
            memcpy(valueOut, cur->data + lhm->keySize, lhm->valueSize);
            return ZZ_OK();
        }
        cur = cur->hashNext;
    }
    return ZZ_ERR("Key not found");
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

zzOpResult zzLinkedHashMapRemove(zzLinkedHashMap *lhm, const void *key) {
    if (!lhm) return ZZ_ERR("LinkedHashMap pointer is NULL");
    if (!key) return ZZ_ERR("Key pointer is NULL");

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
            return ZZ_OK();
        }
        hashCur = &node->hashNext;
    }
    return ZZ_ERR("Key not found");
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

zzOpResult zzLinkedHashMapGetFirst(const zzLinkedHashMap *lhm, void *keyOut, void *valueOut) {
    if (!lhm) return ZZ_ERR("LinkedHashMap pointer is NULL");
    if (!lhm->head) return ZZ_ERR("Map is empty");

    if (keyOut) memcpy(keyOut, lhm->head->data, lhm->keySize);
    if (valueOut) memcpy(valueOut, lhm->head->data + lhm->keySize, lhm->valueSize);
    return ZZ_OK();
}

zzOpResult zzLinkedHashMapGetLast(const zzLinkedHashMap *lhm, void *keyOut, void *valueOut) {
    if (!lhm) return ZZ_ERR("LinkedHashMap pointer is NULL");
    if (!lhm->tail) return ZZ_ERR("Map is empty");

    if (keyOut) memcpy(keyOut, lhm->tail->data, lhm->keySize);
    if (valueOut) memcpy(valueOut, lhm->tail->data + lhm->keySize, lhm->valueSize);
    return ZZ_OK();
}
/**
 * @brief Initializes an iterator for the LinkedHashMap.
 *
 * This function initializes an iterator to traverse the LinkedHashMap in insertion order.
 * The iterator will visit key-value pairs in the order they were inserted.
 *
 * @param[out] it Pointer to the iterator structure to initialize
 * @param[in] lhm Pointer to the LinkedHashMap to iterate over
 */
void zzLinkedHashMapIteratorInit(zzLinkedHashMapIterator *it, const zzLinkedHashMap *lhm) {
    if (!it || !lhm) return;
    
    it->map = lhm;
    it->current = lhm->head;
    it->state = (lhm->head != NULL) ? ZZ_ITER_VALID : ZZ_ITER_END;
}

/**
 * @brief Advances the iterator to the next key-value pair.
 *
 * This function moves the iterator to the next key-value pair in insertion order
 * and copies the current key and value to the output buffers. Returns false when
 * the iterator reaches the end of the map.
 *
 * @param[in,out] it Pointer to the iterator to advance
 * @param[out] keyOut Pointer to a buffer where the current key will be copied
 * @param[out] valueOut Pointer to a buffer where the current value will be copied
 * @return true if a key-value pair was retrieved, false if the iterator reached the end
 */
bool zzLinkedHashMapIteratorNext(zzLinkedHashMapIterator *it, void *keyOut, void *valueOut) {
    if (!it || !keyOut || !valueOut || it->state != ZZ_ITER_VALID || !it->current) return false;
    
    // Copy current key and value
    memcpy(keyOut, it->current->data, it->map->keySize);
    memcpy(valueOut, (char*)it->current->data + it->map->keySize, it->map->valueSize);
    
    // Move to next node in insertion order
    it->current = it->current->next;
    if (!it->current) {
        it->state = ZZ_ITER_END;
    }
    
    return true;
}

/**
 * @brief Checks if the iterator has more elements.
 *
 * This function checks whether the iterator can advance to another key-value pair
 * without actually advancing it.
 *
 * @param[in] it Pointer to the iterator to check
 * @return true if there are more elements, false otherwise
 */
bool zzLinkedHashMapIteratorHasNext(const zzLinkedHashMapIterator *it) {
    return it && it->state == ZZ_ITER_VALID && it->current != NULL;
}
