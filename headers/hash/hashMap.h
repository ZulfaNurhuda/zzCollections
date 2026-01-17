/**
 * @file hashMap.h
 * @brief Hash map implementation using chaining for collision resolution.
 *
 * This module implements a hash map (associative array) that maps keys to values
 * using a hash table with chaining for collision resolution. The implementation
 * provides average O(1) time complexity for lookup, insertion, and deletion
 * operations. The hash map supports generic key and value types and includes
 * automatic resizing when the load factor threshold is exceeded.
 */

#ifndef HASH_MAP_H
#define HASH_MAP_H

#include "types.h"
#include "utils.h"
#include "result.h"

/**
 * @brief Structure representing a node in the hash map's collision chain.
 *
 * Each node contains a pointer to the next node in the chain, the hash value
 * of the key, and a flexible array member to store both key and value data.
 */
typedef struct MapNode {
    struct MapNode *next; /**< Pointer to the next node in the collision chain */
    uint32_t hash;        /**< Hash value of the key for quick comparison */
    unsigned char data[]; /**< Flexible array member to store key and value data */
} MapNode;

/**
 * @brief Structure representing a hash map.
 *
 * This structure maintains an array of buckets (collision chains), tracks the
 * current size and capacity, and provides function pointers for hashing,
 * equality checking, and memory management.
 */
typedef struct zzHashMap {
    MapNode **buckets;   /**< Array of pointers to the heads of collision chains */
    size_t capacity;     /**< Number of buckets in the hash table */
    size_t size;         /**< Current number of key-value pairs in the map */
    size_t keySize;      /**< Size in bytes of each key */
    size_t valueSize;    /**< Size in bytes of each value */
    float loadFactor;    /**< Threshold for triggering resize operations (default 0.75) */
    zzHashFn hashFn;     /**< Function to compute hash values for keys */
    zzEqualsFn equalsFn; /**< Function to compare keys for equality */
    zzFreeFn keyFree;    /**< Function to free key memory, or NULL if not needed */
    zzFreeFn valueFree;  /**< Function to free value memory, or NULL if not needed */
} zzHashMap;

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
zzOpResult zzHashMapInit(zzHashMap *hm, size_t keySize, size_t valueSize, size_t capacity, zzHashFn hashFn, zzEqualsFn equalsFn, zzFreeFn keyFree, zzFreeFn valueFree);

/**
 * @brief Frees all resources associated with the HashMap.
 *
 * This function releases all memory used by the HashMap, including calling
 * the custom free functions for each key and value if provided. After this
 * function returns, the HashMap structure should not be used until reinitialized.
 *
 * @param[in,out] hm Pointer to the HashMap to free
 */
void zzHashMapFree(zzHashMap *hm);

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
zzOpResult zzHashMapPut(zzHashMap *hm, const void *key, const void *value);

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
zzOpResult zzHashMapGet(const zzHashMap *hm, const void *key, void *valueOut);

/**
 * @brief Checks if the HashMap contains the specified key.
 *
 * This function checks whether the given key exists in the hash map.
 *
 * @param[in] hm Pointer to the HashMap to check
 * @param[in] key Pointer to the key to look for
 * @return true if the key exists in the map, false otherwise
 */
bool zzHashMapContains(const zzHashMap *hm, const void *key);

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
zzOpResult zzHashMapRemove(zzHashMap *hm, const void *key);

/**
 * @brief Clears all entries from the HashMap.
 *
 * This function removes all key-value pairs from the hash map by calling the
 * custom free functions for each key and value (if provided) and deallocating
 * all nodes. The hash map becomes empty after this operation.
 *
 * @param[in,out] hm Pointer to the HashMap to clear
 */
void zzHashMapClear(zzHashMap *hm);

#endif