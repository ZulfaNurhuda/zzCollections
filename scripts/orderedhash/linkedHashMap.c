#include "linkedHashMap.h"
#include <string.h>
#include <stdlib.h>

/**
 * @brief Initializes a new LinkedHashMap with the specified key and value sizes.
 *
 * This function initializes a LinkedHashMap structure with the given key and value sizes,
 * initial capacity, and custom functions for hashing, equality checking, and memory
 * management. The linked hash map will be empty after initialization and will maintain
 * insertion order of entries.
 *
 * @param[out] lhm Pointer to the LinkedHashMap structure to initialize
 * @param[in] keySize Size in bytes of each key that will be stored in the map
 * @param[in] valueSize Size in bytes of each value that will be stored in the map
 * @param[in] capacity Initial capacity of the linked hash map (will be adjusted to at least 16 and rounded up to power of 2)
 * @param[in] hashFn Function to compute hash values for keys, or NULL to use default
 * @param[in] equalsFn Function to compare keys for equality, or NULL to use default
 * @param[in] keyFree Function to free key memory when entries are removed or the map is freed, or NULL if not needed
 * @param[in] valueFree Function to free value memory when entries are removed or the map is freed, or NULL if not needed
 * @return zzOpResult with status ZZ_SUCCESS on success, or ZZ_ERROR with error message on failure
 */
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

/**
 * @brief Frees all resources associated with the LinkedHashMap.
 *
 * This function releases all memory used by the LinkedHashMap, including calling
 * the custom free functions for each key and value if provided. After this
 * function returns, the LinkedHashMap structure should not be used until reinitialized.
 *
 * @param[in,out] lhm Pointer to the LinkedHashMap to free
 */
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

/**
 * @brief Inserts or updates a key-value pair in the LinkedHashMap.
 *
 * This function inserts a new key-value pair into the linked hash map, or updates the
 * value if the key already exists. Both the key and value are copied into
 * the map's internal storage. If the load factor threshold is exceeded after
 * the insertion, the linked hash map will be automatically resized. The insertion
 * order is maintained.
 *
 * @param[in,out] lhm Pointer to the LinkedHashMap to insert/update in
 * @param[in] key Pointer to the key to insert/update (contents will be copied)
 * @param[in] value Pointer to the value to insert/update (contents will be copied)
 * @return zzOpResult with status ZZ_SUCCESS on success, or ZZ_ERROR with error message on failure
 */
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

/**
 * @brief Retrieves the value associated with the given key from the LinkedHashMap.
 *
 * This function looks up the specified key in the linked hash map and copies the
 * associated value to the output buffer if found. The function returns an error
 * if the key is not present in the map.
 *
 * @param[in] lhm Pointer to the LinkedHashMap to retrieve from
 * @param[in] key Pointer to the key to look up
 * @param[out] valueOut Pointer to a buffer where the value will be copied if the key is found
 * @return zzOpResult with status ZZ_SUCCESS on success, or ZZ_ERROR with error message on failure
 */
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

/**
 * @brief Checks if the LinkedHashMap contains the specified key.
 *
 * This function checks whether the given key exists in the linked hash map.
 *
 * @param[in] lhm Pointer to the LinkedHashMap to check
 * @param[in] key Pointer to the key to look for
 * @return true if the key exists in the map, false otherwise
 */
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

/**
 * @brief Removes the key-value pair associated with the given key from the LinkedHashMap.
 *
 * This function removes the entry with the specified key from the linked hash map.
 * If a custom free function was provided for keys or values, it will be called
 * on the removed key and value. The function returns an error if the key is not
 * present in the map. The insertion order is maintained after removal.
 *
 * @param[in,out] lhm Pointer to the LinkedHashMap to remove from
 * @param[in] key Pointer to the key to remove
 * @return zzOpResult with status ZZ_SUCCESS on success, or ZZ_ERROR with error message on failure
 */
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

/**
 * @brief Clears all entries from the LinkedHashMap.
 *
 * This function removes all key-value pairs from the linked hash map by calling the
 * custom free functions for each key and value (if provided) and deallocating
 * all nodes. The linked hash map becomes empty after this operation while maintaining
 * its structure.
 *
 * @param[in,out] lhm Pointer to the LinkedHashMap to clear
 */
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

/**
 * @brief Gets the first key-value pair in insertion order from the LinkedHashMap.
 *
 * This function retrieves the first key-value pair that was inserted into the linked hash map
 * and copies both the key and value to the output buffers. The first entry corresponds
 * to the head of the insertion order list.
 *
 * @param[in] lhm Pointer to the LinkedHashMap to retrieve from
 * @param[out] keyOut Pointer to a buffer where the key will be copied
 * @param[out] valueOut Pointer to a buffer where the value will be copied
 * @return zzOpResult with status ZZ_SUCCESS on success, or ZZ_ERROR with error message on failure
 */
zzOpResult zzLinkedHashMapGetFirst(const zzLinkedHashMap *lhm, void *keyOut, void *valueOut) {
    if (!lhm) return ZZ_ERR("LinkedHashMap pointer is NULL");
    if (!lhm->head) return ZZ_ERR("Map is empty");

    if (keyOut) memcpy(keyOut, lhm->head->data, lhm->keySize);
    if (valueOut) memcpy(valueOut, lhm->head->data + lhm->keySize, lhm->valueSize);
    return ZZ_OK();
}

/**
 * @brief Gets the last key-value pair in insertion order from the LinkedHashMap.
 *
 * This function retrieves the last key-value pair that was inserted into the linked hash map
 * and copies both the key and value to the output buffers. The last entry corresponds
 * to the tail of the insertion order list.
 *
 * @param[in] lhm Pointer to the LinkedHashMap to retrieve from
 * @param[out] keyOut Pointer to a buffer where the key will be copied
 * @param[out] valueOut Pointer to a buffer where the value will be copied
 * @return zzOpResult with status ZZ_SUCCESS on success, or ZZ_ERROR with error message on failure
 */
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
void zzLinkedHashMapIteratorInit(zzLinkedHashMapIterator *it, zzLinkedHashMap *lhm) {
    if (!it || !lhm) return;
    
    it->map = lhm;
    it->current = lhm->head;
    it->lastReturned = NULL;
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
    
    memcpy(keyOut, it->current->data, it->map->keySize);
    memcpy(valueOut, (char*)it->current->data + it->map->keySize, it->map->valueSize);
    
    it->lastReturned = it->current;

    it->current = it->current->next;
    
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

/**
 * @brief Removes the last key-value pair returned by the iterator.
 *
 * This function removes the key-value pair that was most recently returned by
 * zzLinkedHashMapIteratorNext. After removal, the iterator remains valid and
 * continues to the next element on the next call to Next.
 *
 * @param[in,out] it Pointer to the iterator
 * @return zzOpResult with status ZZ_SUCCESS on success, or ZZ_ERROR with error message on failure
 */
zzOpResult zzLinkedHashMapIteratorRemove(zzLinkedHashMapIterator *it) {
    if (!it || it->state != ZZ_ITER_VALID) return ZZ_ERR("Invalid iterator state");
    if (!it->lastReturned) return ZZ_ERR("No element to remove");

    LHMapNode *target = it->lastReturned;
    zzLinkedHashMap *lhm = it->map;

    if (target->prev) target->prev->next = target->next;
    else lhm->head = target->next;

    if (target->next) target->next->prev = target->prev;
    else lhm->tail = target->prev;

    size_t bucketIdx = target->hash % lhm->capacity;
    LHMapNode **cur = &lhm->buckets[bucketIdx];
    while (*cur) {
        if (*cur == target) {
            *cur = target->hashNext;
            break;
        }
        cur = &(*cur)->hashNext;
    }

    if (lhm->keyFree) lhm->keyFree(target->data);
    if (lhm->valueFree) lhm->valueFree(target->data + lhm->keySize);
    
    free(target);
    lhm->size--;
    it->lastReturned = NULL;
    
    return ZZ_OK();
}