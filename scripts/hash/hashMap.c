/**
 * @file hashMap.c
 * @brief Hash map implementation with separate chaining for collision resolution
 *
 * Source implementation for the hash map data structure that provides
 * O(1) average time complexity for insert, lookup, and delete operations.
 * Uses separate chaining with linked lists to handle hash collisions.
 */

#include "hashMap.h"
#include <string.h>
#include <stdlib.h>

/**
 * @brief Initialize a hash map with specified parameters
 *
 * Sets up the internal structure of the hash map including allocating the bucket array
 * and initializing all fields. If hashFn or equalsFn are NULL, default implementations
 * are used.
 *
 * @param[out] hm Pointer to the hash map to initialize
 * @param[in] keySize Size of each key in bytes (must be > 0)
 * @param[in] valueSize Size of each value in bytes (must be > 0)
 * @param[in] capacity Initial number of buckets (use 0 for default of 16)
 * @param[in] hashFn Hash function for keys
 * @param[in] equalsFn Equality function for key comparison
 * @param[in] keyFree Optional cleanup function for keys (can be NULL)
 * @param[in] valueFree Optional cleanup function for values (can be NULL)
 *
 * @return zzOpResult with status:
 *         - SUCCESS: Hash map initialized successfully
 *         - ERROR: Possible errors:
 *           * "HashMap pointer is NULL"
 *           * "Key size cannot be zero"
 *           * "Value size cannot be zero"
 *           * "Hash function is NULL"
 *           * "Equals function is NULL"
 *           * "Failed to allocate buckets"
 *
 * @see zzHashMapFree
 */
zzOpResult zzHashMapInit(zzHashMap *hm, size_t keySize, size_t valueSize, size_t capacity,
                 zzHashFn hashFn, zzEqualsFn equalsFn, zzFreeFn keyFree, zzFreeFn valueFree) {
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

/**
 * @brief Free all memory associated with the hash map
 *
 * Iterates through all buckets and nodes, calling the keyFree and valueFree callbacks
 * if provided during initialization. Then frees all allocated memory for nodes and buckets.
 * The hash map struct itself is not freed (caller owns it).
 *
 * @param[in,out] hm Pointer to the hash map to free
 *
 * @note Safe to call with NULL pointer (no-op)
 */
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

/**
 * @brief Internal function to resize the hash map when load factor threshold is exceeded
 *
 * Doubles the capacity of the hash map and redistributes all existing entries
 * to their new positions based on the new capacity. This maintains O(1) average
 * time complexity by preventing the load factor from becoming too high.
 *
 * @param[in,out] hm Pointer to the hash map to rehash
 *
 * @return zzOpResult with status:
 *         - SUCCESS: Rehash completed successfully
 *         - ERROR: "Failed to rehash (out of memory)" if allocation fails
 *
 * @note This is an internal function and should not be called directly
 */
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

/**
 * @brief Insert or update a key-value pair in the hash map
 *
 * Computes the hash of the key and inserts the key-value pair into the appropriate bucket.
 * If the key already exists, its value will be updated (old value is freed if valueFree provided).
 * Triggers rehashing if the load factor exceeds the threshold.
 *
 * @param[in,out] hm Pointer to the hash map
 * @param[in] key Pointer to the key data to insert/update
 * @param[in] value Pointer to the value data to associate with key
 *
 * @return zzOpResult with status:
 *         - SUCCESS: Key-value pair inserted/updated successfully
 *         - ERROR: Possible errors:
 *           * "HashMap pointer is NULL"
 *           * "Key pointer is NULL"
 *           * "Value pointer is NULL"
 *           * "Failed to allocate node"
 *
 * @see zzHashMapGet
 * @see zzHashMapRemove
 */
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

/**
 * @brief Retrieve a value by its key
 *
 * Searches for the specified key in the hash map and copies the associated value
 * to the output buffer if found. Uses the hash function to determine the bucket
 * and the equals function to check for key equality in case of collisions.
 *
 * @param[in] hm Pointer to the hash map
 * @param[in] key Pointer to the key to search for
 * @param[out] valueOut Pointer to buffer where value will be copied
 *
 * @return zzOpResult with status:
 *         - SUCCESS: Key found and value copied to valueOut
 *         - ERROR: Possible errors:
 *           * "HashMap pointer is NULL"
 *           * "Key pointer is NULL"
 *           * "Value output buffer is NULL"
 *           * "Key not found"
 *
 * @see zzHashMapPut
 * @see zzHashMapContains
 */
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

/**
 * @brief Check if a key exists in the hash map
 *
 * Determines whether the specified key exists in the hash map without retrieving its value.
 * Uses the hash function to determine the bucket and the equals function to check for key
 * equality in case of collisions.
 *
 * @param[in] hm Pointer to the hash map
 * @param[in] key Pointer to the key to check
 *
 * @return true if key exists, false otherwise
 */
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

/**
 * @brief Remove a key-value pair from the hash map
 *
 * Searches for the specified key in the hash map and removes the corresponding entry.
 * Calls the keyFree and valueFree callbacks if provided during initialization to clean up
 * the key and value data before freeing the node memory.
 *
 * @param[in,out] hm Pointer to the hash map
 * @param[in] key Pointer to the key to remove
 *
 * @return zzOpResult with status:
 *         - SUCCESS: Key-value pair removed successfully
 *         - ERROR: Possible errors:
 *           * "HashMap pointer is NULL"
 *           * "Key pointer is NULL"
 *           * "Key not found"
 *
 * @see zzHashMapPut
 */
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

/**
 * @brief Remove all key-value pairs from the hash map
 *
 * Empties the hash map by removing all entries and calling the keyFree and valueFree
 * callbacks if provided during initialization. The bucket array is preserved but emptied.
 *
 * @param[in,out] hm Pointer to the hash map to clear
 *
 * @note Safe to call with NULL pointer (no-op)
 */
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
