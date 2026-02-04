#include "linkedHashSet.h"
#include <string.h>
#include <stdlib.h>

/**
 * @brief Initializes a new LinkedHashSet with the specified key size.
 *
 * This function initializes a LinkedHashSet structure with the given key size,
 * initial capacity, and custom functions for hashing, equality checking, and memory
 * management. The linked hash set will be empty after initialization and will maintain
 * insertion order of keys.
 *
 * @param[out] lhs Pointer to the LinkedHashSet structure to initialize
 * @param[in] keySize Size in bytes of each key that will be stored in the set
 * @param[in] capacity Initial capacity of the linked hash set (will be adjusted to at least 16)
 * @param[in] hashFn Function to compute hash values for keys, or NULL to use default
 * @param[in] equalsFn Function to compare keys for equality, or NULL to use default
 * @param[in] keyFree Function to free key memory when entries are removed or the set is freed, or NULL if not needed
 * @return zzOpResult with status ZZ_SUCCESS on success, or ZZ_ERROR with error message on failure
 */
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

/**
 * @brief Frees all resources associated with the LinkedHashSet.
 *
 * This function releases all memory used by the LinkedHashSet, including calling
 * the custom free function for each key if provided. After this function
 * returns, the LinkedHashSet structure should not be used until reinitialized.
 *
 * @param[in,out] lhs Pointer to the LinkedHashSet to free
 */
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

/**
 * @brief Inserts a key into the LinkedHashSet.
 *
 * This function inserts a new key into the linked hash set if it doesn't already exist.
 * The key is copied into the set's internal storage. If the load factor threshold
 * is exceeded after the insertion, the linked hash set will be automatically resized.
 * The insertion order is maintained.
 *
 * @param[in,out] lhs Pointer to the LinkedHashSet to insert into
 * @param[in] key Pointer to the key to insert (contents will be copied)
 * @return zzOpResult with status ZZ_SUCCESS on success, or ZZ_ERROR with error message on failure
 */
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

/**
 * @brief Checks if the LinkedHashSet contains the specified key.
 *
 * This function checks whether the given key exists in the linked hash set.
 *
 * @param[in] lhs Pointer to the LinkedHashSet to check
 * @param[in] key Pointer to the key to look for
 * @return true if the key exists in the set, false otherwise
 */
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

/**
 * @brief Removes a key from the LinkedHashSet.
 *
 * This function removes the specified key from the linked hash set if it exists.
 * If a custom free function was provided for keys, it will be called on
 * the removed key. The function returns an error if the key is not present
 * in the set. The insertion order is maintained after removal.
 *
 * @param[in,out] lhs Pointer to the LinkedHashSet to remove from
 * @param[in] key Pointer to the key to remove
 * @return zzOpResult with status ZZ_SUCCESS on success, or ZZ_ERROR with error message on failure
 */
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

/**
 * @brief Clears all keys from the LinkedHashSet.
 *
 * This function removes all keys from the linked hash set by calling the custom free
 * function for each key (if provided) and deallocating all nodes. The linked hash set
 * becomes empty after this operation while maintaining its structure.
 *
 * @param[in,out] lhs Pointer to the LinkedHashSet to clear
 */
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

/**
 * @brief Gets the first key in insertion order from the LinkedHashSet.
 *
 * This function retrieves the first key that was inserted into the linked hash set
 * and copies it to the output buffer. The first key corresponds
 * to the head of the insertion order list.
 *
 * @param[in] lhs Pointer to the LinkedHashSet to retrieve from
 * @param[out] keyOut Pointer to a buffer where the key will be copied
 * @return zzOpResult with status ZZ_SUCCESS on success, or ZZ_ERROR with error message on failure
 */
zzOpResult zzLinkedHashSetGetFirst(const zzLinkedHashSet *lhs, void *keyOut) {
    if (!lhs) return ZZ_ERR("LinkedHashSet pointer is NULL");
    if (!keyOut) return ZZ_ERR("Key output pointer is NULL");
    if (!lhs->head) return ZZ_ERR("Set is empty");

    memcpy(keyOut, lhs->head->key, lhs->keySize);
    return ZZ_OK();
}

/**
 * @brief Gets the last key in insertion order from the LinkedHashSet.
 *
 * This function retrieves the last key that was inserted into the linked hash set
 * and copies it to the output buffer. The last key corresponds
 * to the tail of the insertion order list.
 *
 * @param[in] lhs Pointer to the LinkedHashSet to retrieve from
 * @param[out] keyOut Pointer to a buffer where the key will be copied
 * @return zzOpResult with status ZZ_SUCCESS on success, or ZZ_ERROR with error message on failure
 */
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
void zzLinkedHashSetIteratorInit(zzLinkedHashSetIterator *it, zzLinkedHashSet *lhs) {
    if (!it || !lhs) return;
    
    it->set = lhs;
    it->current = lhs->head;
    it->lastReturned = NULL;
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
    
    memcpy(keyOut, it->current->key, it->set->keySize);
    
    it->lastReturned = it->current;

    it->current = it->current->next;
    
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

/**
 * @brief Removes the last key returned by the iterator.
 *
 * This function removes the key that was most recently returned by
 * zzLinkedHashSetIteratorNext. After removal, the iterator remains valid and
 * continues to the next element on the next call to Next.
 *
 * @param[in,out] it Pointer to the iterator
 * @return zzOpResult with status ZZ_SUCCESS on success, or ZZ_ERROR with error message on failure
 */
zzOpResult zzLinkedHashSetIteratorRemove(zzLinkedHashSetIterator *it) {
    if (!it || it->state != ZZ_ITER_VALID) return ZZ_ERR("Invalid iterator state");
    if (!it->lastReturned) return ZZ_ERR("No element to remove");

    LHSetNode *target = it->lastReturned;
    zzLinkedHashSet *lhs = it->set;

    if (target->prev) target->prev->next = target->next;
    else lhs->head = target->next;

    if (target->next) target->next->prev = target->prev;
    else lhs->tail = target->prev;

    size_t bucketIdx = target->hash % lhs->capacity;
    LHSetNode **cur = &lhs->buckets[bucketIdx];
    while (*cur) {
        if (*cur == target) {
            *cur = target->hashNext;
            break;
        }
        cur = &(*cur)->hashNext;
    }

    if (lhs->keyFree) lhs->keyFree(target->key);
    
    free(target);
    lhs->size--;
    it->lastReturned = NULL;
    
    return ZZ_OK();
}