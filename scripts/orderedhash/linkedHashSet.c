#include "linkedHashSet.h"
#include <string.h>
#include <stdlib.h>

zzOpResult zzLinkedHashSetInit(zzLinkedHashSet *lhs, size_t keySize, size_t capacity, zzHashFn hashFn, zzEqualsFn equalsFn, zzFreeFn keyFree) {
    if (!lhs) return ZZ_ERR("LinkedHashSet pointer is NULL");
    if (keySize == 0) return ZZ_ERR("Key size cannot be zero");
    if (capacity == 0) capacity = 16;

    lhs->buckets = calloc(capacity, sizeof(LHSetNode*));
    if (!lhs->buckets) return ZZ_ERR("Failed to allocate buckets");

    lhs->head = NULL;
    lhs->tail = NULL;
    lhs->keySize = keySize;
    lhs->capacity = capacity;
    lhs->size = 0;
    lhs->loadFactor = 0.75f;
    lhs->hashFn = hashFn ? hashFn : zzDefaultHash;
    lhs->equalsFn = equalsFn ? equalsFn : zzDefaultEquals;
    lhs->keyFree = keyFree;
    return ZZ_OK();
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

static zzOpResult zzLinkedHashSetRehash(zzLinkedHashSet *lhs) {
    size_t newCap = lhs->capacity * 2;
    LHSetNode **newBuckets = calloc(newCap, sizeof(LHSetNode*));
    if (!newBuckets) return ZZ_ERR("Failed to rehash (out of memory)");

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
    return ZZ_OK();
}

zzOpResult zzLinkedHashSetInsert(zzLinkedHashSet *lhs, const void *key) {
    if (!lhs) return ZZ_ERR("LinkedHashSet pointer is NULL");
    if (!key) return ZZ_ERR("Key pointer is NULL");

    uint32_t hash = lhs->hashFn(key);
    size_t idx = hash % lhs->capacity;

    LHSetNode *cur = lhs->buckets[idx];
    while (cur) {
        if (cur->hash == hash && lhs->equalsFn(cur->key, key))
            return ZZ_ERR("Key already exists");
        cur = cur->hashNext;
    }

    LHSetNode *node = malloc(sizeof(LHSetNode) + lhs->keySize);
    if (!node) return ZZ_ERR("Failed to allocate node");

    node->hash = hash;
    memcpy(node->key, key, lhs->keySize);

    node->hashNext = lhs->buckets[idx];
    lhs->buckets[idx] = node;

    node->prev = lhs->tail;
    node->next = NULL;
    if (lhs->tail) lhs->tail->next = node;
    lhs->tail = node;
    if (!lhs->head) lhs->head = node;

    lhs->size++;

    if ((float)lhs->size / lhs->capacity > lhs->loadFactor) {
        zzOpResult rehashResult = zzLinkedHashSetRehash(lhs);
        if (ZZ_IS_ERR(rehashResult)) return rehashResult;
    }

    return ZZ_OK();
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

zzOpResult zzLinkedHashSetRemove(zzLinkedHashSet *lhs, const void *key) {
    if (!lhs) return ZZ_ERR("LinkedHashSet pointer is NULL");
    if (!key) return ZZ_ERR("Key pointer is NULL");

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
            return ZZ_OK();
        }
        hashCur = &node->hashNext;
    }
    return ZZ_ERR("Key not found");
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

zzOpResult zzLinkedHashSetGetFirst(const zzLinkedHashSet *lhs, void *keyOut) {
    if (!lhs) return ZZ_ERR("LinkedHashSet pointer is NULL");
    if (!keyOut) return ZZ_ERR("Key output pointer is NULL");
    if (!lhs->head) return ZZ_ERR("Set is empty");

    memcpy(keyOut, lhs->head->key, lhs->keySize);
    return ZZ_OK();
}

zzOpResult zzLinkedHashSetGetLast(const zzLinkedHashSet *lhs, void *keyOut) {
    if (!lhs) return ZZ_ERR("LinkedHashSet pointer is NULL");
    if (!keyOut) return ZZ_ERR("Key output pointer is NULL");
    if (!lhs->tail) return ZZ_ERR("Set is empty");

    memcpy(keyOut, lhs->tail->key, lhs->keySize);
    return ZZ_OK();
}
/**
 * @brief Initializes an iterator for the LinkedHashSet.
 *
 * This function initializes an iterator to traverse the LinkedHashSet in insertion order.
 * The iterator will visit keys in the order they were inserted.
 *
 * @param[out] it Pointer to the iterator structure to initialize
 * @param[in] lhs Pointer to the LinkedHashSet to iterate over
 */
void zzLinkedHashSetIteratorInit(zzLinkedHashSetIterator *it, const zzLinkedHashSet *lhs) {
    if (!it || !lhs) return;
    
    it->set = lhs;
    it->current = lhs->head;
    it->state = (lhs->head != NULL) ? ZZ_ITER_VALID : ZZ_ITER_END;
}

/**
 * @brief Advances the iterator to the next key.
 *
 * This function moves the iterator to the next key in insertion order
 * and copies the current key to the output buffer. Returns false when
 * the iterator reaches the end of the set.
 *
 * @param[in,out] it Pointer to the iterator to advance
 * @param[out] keyOut Pointer to a buffer where the current key will be copied
 * @return true if a key was retrieved, false if the iterator reached the end
 */
bool zzLinkedHashSetIteratorNext(zzLinkedHashSetIterator *it, void *keyOut) {
    if (!it || !keyOut || it->state != ZZ_ITER_VALID || !it->current) return false;
    
    // Copy current key
    memcpy(keyOut, it->current->key, it->set->keySize);
    
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
 * This function checks whether the iterator can advance to another key
 * without actually advancing it.
 *
 * @param[in] it Pointer to the iterator to check
 * @return true if there are more elements, false otherwise
 */
bool zzLinkedHashSetIteratorHasNext(const zzLinkedHashSetIterator *it) {
    return it && it->state == ZZ_ITER_VALID && it->current != NULL;
}
