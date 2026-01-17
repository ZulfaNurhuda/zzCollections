/**
 * @file hashMap.c
 * @brief Implementation of the hash map data structure using chaining for collision resolution.
 *
 * This module provides the implementation for the HashMap data structure,
 * which offers efficient key-value mapping with average O(1) time complexity
 * for lookup, insertion, and deletion operations. The implementation handles
 * memory management, collision resolution through chaining, and automatic
 * resizing when the load factor threshold is exceeded.
 */

#include "hashMap.h"
#include <string.h>
#include <stdlib.h>

/**
 * @brief Initializes a new HashMap with the specified key and value sizes.
 *
 * This function initializes a HashMap structure with the given key and value sizes,
 * initial capacity, and custom functions for hashing, equality checking, and memory
 * management. The hash map will be empty after initialization.
 *
 * @param[out] hm Pointer to the HashMap structure to initialize
 * @param[in] keySize Size in bytes of each key that will be stored in the map
 * @param[in] valueSize Size in bytes of each value that will be stored in the map
 * @param[in] capacity Initial capacity of the hash map (will be adjusted to at least 16 and rounded up to power of 2)
 * @param[in] hashFn Function to compute hash values for keys, or NULL to use default
 * @param[in] equalsFn Function to compare keys for equality, or NULL to use default
 * @param[in] keyFree Function to free key memory when entries are removed or the map is freed, or NULL if not needed
 * @param[in] valueFree Function to free value memory when entries are removed or the map is freed, or NULL if not needed
 * @return zzOpResult with status ZZ_SUCCESS on success, or ZZ_ERROR with error message on failure
 */
zzOpResult zzHashMapInit(zzHashMap *hm, size_t keySize, size_t valueSize, size_t capacity, zzHashFn hashFn, zzEqualsFn equalsFn, zzFreeFn keyFree, zzFreeFn valueFree) {
    if (!hm) return ZZ_ERR("HashMap pointer is NULL");
    if (keySize == 0) return ZZ_ERR("Key size cannot be zero");
    if (valueSize == 0) return ZZ_ERR("Value size cannot be zero");
    if (capacity == 0) capacity = 16;

    capacity = capacity < 16 ? 16 : capacity;
    if (capacity & (capacity - 1)) {
        capacity--;
        capacity |= capacity >> 1;
        capacity |= capacity >> 2;
        capacity |= capacity >> 4;
        capacity |= capacity >> 8;
        capacity |= capacity >> 16;
        capacity++;
    }

    hm->buckets = calloc(capacity, sizeof(MapNode*));
    if (!hm->buckets) return ZZ_ERR("Failed to allocate buckets");

    hm->capacity = capacity;
    hm->size = 0;
    hm->keySize = keySize;
    hm->valueSize = valueSize;
    hm->loadFactor = 0.75f;
    hm->hashFn = hashFn ? hashFn : zzDefaultHash;
    hm->equalsFn = equalsFn ? equalsFn : zzDefaultEquals;
    hm->keyFree = keyFree;
    hm->valueFree = valueFree;
    return ZZ_OK();
}

/**
 * @brief Frees all resources associated with the HashMap.
 *
 * This function releases all memory used by the HashMap, including calling
 * the custom free functions for each key and value if provided. After this
 * function returns, the HashMap structure should not be used until reinitialized.
 *
 * @param[in,out] hm Pointer to the HashMap to free
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
 * @brief Internal function to rehash the HashMap when load factor threshold is exceeded.
 *
 * This helper function doubles the capacity of the hash map and redistributes
 * all existing entries to their new positions based on the new hash table size.
 * This maintains the efficiency of the hash map by keeping the load factor low.
 *
 * @param[in,out] hm Pointer to the HashMap to rehash
 * @return zzOpResult with status ZZ_SUCCESS on success, or ZZ_ERROR with error message on failure
 */
static zzOpResult zzHashMapRehash(zzHashMap *hm) {
    size_t newCap = hm->capacity << 1;
    MapNode **newBuckets = calloc(newCap, sizeof(MapNode*));
    if (!newBuckets) return ZZ_ERR("Failed to rehash (out of memory)");

    for (size_t i = 0; i < hm->capacity; i++) {
        MapNode *cur = hm->buckets[i];
        while (cur) {
            MapNode *next = cur->next;
            size_t idx = cur->hash & (newCap - 1);
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
 * @brief Inserts or updates a key-value pair in the HashMap.
 *
 * This function inserts a new key-value pair into the hash map, or updates the
 * value if the key already exists. Both the key and value are copied into
 * the map's internal storage. If the load factor threshold is exceeded after
 * the insertion, the hash map will be automatically resized.
 *
 * @param[in,out] hm Pointer to the HashMap to insert/update in
 * @param[in] key Pointer to the key to insert/update (contents will be copied)
 * @param[in] value Pointer to the value to insert/update (contents will be copied)
 * @return zzOpResult with status ZZ_SUCCESS on success, or ZZ_ERROR with error message on failure
 */
zzOpResult zzHashMapPut(zzHashMap *hm, const void *key, const void *value) {
    if (!hm) return ZZ_ERR("HashMap pointer is NULL");
    if (!key) return ZZ_ERR("Key pointer is NULL");
    if (!value) return ZZ_ERR("Value pointer is NULL");

    uint32_t hash = hm->hashFn(key);
    size_t idx = hash & (hm->capacity - 1);

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
 * @brief Retrieves the value associated with the given key from the HashMap.
 *
 * This function looks up the specified key in the hash map and copies the
 * associated value to the output buffer if found. The function returns an error
 * if the key is not present in the map.
 *
 * @param[in] hm Pointer to the HashMap to retrieve from
 * @param[in] key Pointer to the key to look up
 * @param[out] valueOut Pointer to a buffer where the value will be copied if the key is found
 * @return zzOpResult with status ZZ_SUCCESS on success, or ZZ_ERROR with error message on failure
 */
zzOpResult zzHashMapGet(const zzHashMap *hm, const void *key, void *valueOut) {
    if (!hm) return ZZ_ERR("HashMap pointer is NULL");
    if (!key) return ZZ_ERR("Key pointer is NULL");
    if (!valueOut) return ZZ_ERR("Value output buffer is NULL");

    uint32_t hash = hm->hashFn(key);
    size_t idx = hash & (hm->capacity - 1);

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
 * @brief Checks if the HashMap contains the specified key.
 *
 * This function checks whether the given key exists in the hash map.
 *
 * @param[in] hm Pointer to the HashMap to check
 * @param[in] key Pointer to the key to look for
 * @return true if the key exists in the map, false otherwise
 */
bool zzHashMapContains(const zzHashMap *hm, const void *key) {
    if (!hm || !key) return false;

    uint32_t hash = hm->hashFn(key);
    size_t idx = hash & (hm->capacity - 1);

    MapNode *cur = hm->buckets[idx];
    while (cur) {
        if (cur->hash == hash && hm->equalsFn(cur->data, key))
            return true;
        cur = cur->next;
    }
    return false;
}

/**
 * @brief Removes the key-value pair associated with the given key from the HashMap.
 *
 * This function removes the entry with the specified key from the hash map.
 * If a custom free function was provided for keys or values, it will be called
 * on the removed key and value. The function returns an error if the key is not
 * present in the map.
 *
 * @param[in,out] hm Pointer to the HashMap to remove from
 * @param[in] key Pointer to the key to remove
 * @return zzOpResult with status ZZ_SUCCESS on success, or ZZ_ERROR with error message on failure
 */
zzOpResult zzHashMapRemove(zzHashMap *hm, const void *key) {
    if (!hm) return ZZ_ERR("HashMap pointer is NULL");
    if (!key) return ZZ_ERR("Key pointer is NULL");

    uint32_t hash = hm->hashFn(key);
    size_t idx = hash & (hm->capacity - 1);

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
 * @brief Clears all entries from the HashMap.
 *
 * This function removes all key-value pairs from the hash map by calling the
 * custom free functions for each key and value (if provided) and deallocating
 * all nodes. The hash map becomes empty after this operation.
 *
 * @param[in,out] hm Pointer to the HashMap to clear
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