/**
 * @file hashMap.h
 * @brief Hash map implementation for key-value storage
 *
 * Provides O(1) average time complexity for insert, lookup, and delete operations.
 * Uses separate chaining with linked lists to handle hash collisions.
 */

#ifndef HASH_MAP_H
#define HASH_MAP_H

#include "types.h"
#include "utils.h"
#include "result.h"

/**
 * @brief Internal node structure for hash map entries
 *
 * Each node contains a pointer to the next node for collision resolution,
 * the cached hash value, and flexible array member for key-value data.
 */
typedef struct MapNode {
    struct MapNode *next;    /**< Next node in collision chain */
    uint32_t hash;          /**< Cached hash value for efficiency */
    unsigned char data[];   /**< Flexible array for key-value storage */
} MapNode;

/**
 * @brief Hash map implementation with separate chaining for collision resolution
 *
 * Provides O(1) average time complexity for insert, lookup, and delete operations.
 * Automatically resizes when load factor threshold is exceeded.
 */
typedef struct zzHashMap {
    MapNode **buckets;      /**< Array of bucket pointers */
    size_t keySize;         /**< Size of each key in bytes */
    size_t valueSize;       /**< Size of each value in bytes */
    size_t capacity;        /**< Number of buckets */
    size_t size;           /**< Current number of key-value pairs */
    float loadFactor;      /**< Load factor threshold for resizing */
    zzHashFn hashFn;       /**< Hash function for keys */
    zzEqualsFn equalsFn;   /**< Equality function for keys */
    zzFreeFn keyFree;      /**< Optional cleanup function for keys */
    zzFreeFn valueFree;    /**< Optional cleanup function for values */
} zzHashMap;

/**
 * @brief Initialize a hash map with specified parameters
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
                 zzHashFn hashFn, zzEqualsFn equalsFn, zzFreeFn keyFree, zzFreeFn valueFree);

/**
 * @brief Free all memory associated with the hash map
 *
 * Calls cleanup functions for keys and values if provided during initialization.
 * The hash map struct itself is not freed (caller owns it).
 *
 * @param[in,out] hm Pointer to the hash map to free
 *
 * @note Safe to call with NULL pointer (no-op)
 */
void zzHashMapFree(zzHashMap *hm);

/**
 * @brief Insert or update a key-value pair in the hash map
 *
 * If the key already exists, its value will be updated (old value is freed if valueFree provided).
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
zzOpResult zzHashMapPut(zzHashMap *hm, const void *key, const void *value);

/**
 * @brief Retrieve a value by its key
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
zzOpResult zzHashMapGet(const zzHashMap *hm, const void *key, void *valueOut);

/**
 * @brief Check if a key exists in the hash map
 *
 * @param[in] hm Pointer to the hash map
 * @param[in] key Pointer to the key to check
 *
 * @return true if key exists, false otherwise
 */
bool zzHashMapContains(const zzHashMap *hm, const void *key);

/**
 * @brief Remove a key-value pair from the hash map
 *
 * Calls cleanup functions for the key and value if provided during initialization.
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
zzOpResult zzHashMapRemove(zzHashMap *hm, const void *key);

/**
 * @brief Remove all key-value pairs from the hash map
 *
 * Calls cleanup functions for all keys and values if provided during initialization.
 *
 * @param[in,out] hm Pointer to the hash map to clear
 *
 * @note Safe to call with NULL pointer (no-op)
 */
void zzHashMapClear(zzHashMap *hm);

#endif /* HASH_MAP_H */
