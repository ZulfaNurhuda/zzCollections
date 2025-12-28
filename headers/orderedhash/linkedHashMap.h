#ifndef LINKED_HASH_MAP_H
#define LINKED_HASH_MAP_H

#include "common.h"
#include "result.h"

typedef struct LHMapNode {
    struct LHMapNode *hashNext;
    struct LHMapNode *prev;
    struct LHMapNode *next;
    uint32_t hash;
    unsigned char data[];
} LHMapNode;

/**
 * @brief LinkedHashMap maintains insertion order with hash table performance
 *
 * Combines hash table with doubly-linked list to preserve insertion order.
 * Provides O(1) access like HashMap plus ordered iteration.
 */
typedef struct zzLinkedHashMap {
    LHMapNode **buckets;
    LHMapNode *head;
    LHMapNode *tail;
    size_t keySize;
    size_t valueSize;
    size_t capacity;
    size_t size;
    float loadFactor;
    zzHashFn hashFn;
    zzEqualsFn equalsFn;
    zzFreeFn keyFree;
    zzFreeFn valueFree;
} zzLinkedHashMap;

/**
 * @brief Initialize LinkedHashMap with specified sizes and capacity
 *
 * Allocates hash table and sets up linked list structure.
 * Must be freed with zzLinkedHashMapFree when done.
 *
 * @param[out] lhm Pointer to LinkedHashMap to initialize
 * @param[in] keySize Size of each key in bytes (must be > 0)
 * @param[in] valueSize Size of each value in bytes (must be > 0)
 * @param[in] capacity Initial capacity (use 0 for default of 16)
 * @param[in] hashFn Hash function (NULL for default)
 * @param[in] equalsFn Equality function (NULL for default)
 * @param[in] keyFree Optional key cleanup function (can be NULL)
 * @param[in] valueFree Optional value cleanup function (can be NULL)
 *
 * @return zzOpResult with status:
 *         - SUCCESS: LinkedHashMap initialized successfully
 *         - ERROR: Possible errors:
 *           * "LinkedHashMap pointer is NULL"
 *           * "Key size cannot be zero"
 *           * "Value size cannot be zero"
 *           * "Failed to allocate buckets"
 *
 * @see zzLinkedHashMapFree
 */
zzOpResult zzLinkedHashMapInit(zzLinkedHashMap *lhm, size_t keySize, size_t valueSize, size_t capacity,
                       zzHashFn hashFn, zzEqualsFn equalsFn, zzFreeFn keyFree, zzFreeFn valueFree);

/**
 * @brief Free all memory associated with LinkedHashMap
 *
 * Calls keyFree and valueFree on all entries (if provided), then frees
 * internal structures. The LinkedHashMap struct itself is not freed.
 *
 * @param[in,out] lhm Pointer to LinkedHashMap to free
 *
 * @note Safe to call with NULL pointer (no-op)
 */
void zzLinkedHashMapFree(zzLinkedHashMap *lhm);

/**
 * @brief Put key-value pair into map
 *
 * Inserts new entry or updates existing one. New entries are added
 * to the end of the insertion-order list. Updates do not change order.
 * May trigger rehash if load factor exceeded.
 *
 * @param[in,out] lhm Pointer to LinkedHashMap
 * @param[in] key Pointer to key data (will be copied)
 * @param[in] value Pointer to value data (will be copied)
 *
 * @return zzOpResult with status:
 *         - SUCCESS: Entry added or updated successfully
 *         - ERROR: Possible errors:
 *           * "LinkedHashMap pointer is NULL"
 *           * "Key pointer is NULL"
 *           * "Value pointer is NULL"
 *           * "Failed to allocate node"
 *           * "Failed to rehash (out of memory)"
 */
zzOpResult zzLinkedHashMapPut(zzLinkedHashMap *lhm, const void *key, const void *value);

/**
 * @brief Get value by key
 *
 * Retrieves value associated with key without affecting order.
 *
 * @param[in] lhm Pointer to LinkedHashMap
 * @param[in] key Pointer to key to search for
 * @param[out] valueOut Buffer where value will be copied (must be valueSize bytes)
 *
 * @return zzOpResult with status:
 *         - SUCCESS: Value retrieved successfully
 *         - ERROR: Possible errors:
 *           * "LinkedHashMap pointer is NULL"
 *           * "Key pointer is NULL"
 *           * "Value output pointer is NULL"
 *           * "Key not found"
 */
zzOpResult zzLinkedHashMapGet(const zzLinkedHashMap *lhm, const void *key, void *valueOut);

/**
 * @brief Check if key exists in map
 *
 * @param[in] lhm Pointer to LinkedHashMap
 * @param[in] key Pointer to key to check
 *
 * @return true if key exists, false otherwise
 */
bool zzLinkedHashMapContains(const zzLinkedHashMap *lhm, const void *key);

/**
 * @brief Remove key-value pair from map
 *
 * Removes entry from both hash table and insertion-order list.
 * Calls keyFree and valueFree if provided.
 *
 * @param[in,out] lhm Pointer to LinkedHashMap
 * @param[in] key Pointer to key to remove
 *
 * @return zzOpResult with status:
 *         - SUCCESS: Entry removed successfully
 *         - ERROR: Possible errors:
 *           * "LinkedHashMap pointer is NULL"
 *           * "Key pointer is NULL"
 *           * "Key not found"
 */
zzOpResult zzLinkedHashMapRemove(zzLinkedHashMap *lhm, const void *key);

/**
 * @brief Clear all entries from map
 *
 * Removes all entries (calls keyFree and valueFree if provided).
 * Keeps allocated buckets. Size becomes 0, capacity unchanged.
 *
 * @param[in,out] lhm Pointer to LinkedHashMap
 */
void zzLinkedHashMapClear(zzLinkedHashMap *lhm);

/**
 * @brief Get first entry in insertion order
 *
 * Retrieves the oldest entry (first inserted).
 *
 * @param[in] lhm Pointer to LinkedHashMap
 * @param[out] keyOut Buffer for key (can be NULL to skip)
 * @param[out] valueOut Buffer for value (can be NULL to skip)
 *
 * @return zzOpResult with status:
 *         - SUCCESS: First entry retrieved successfully
 *         - ERROR: Possible errors:
 *           * "LinkedHashMap pointer is NULL"
 *           * "Map is empty"
 */
zzOpResult zzLinkedHashMapGetFirst(const zzLinkedHashMap *lhm, void *keyOut, void *valueOut);

/**
 * @brief Get last entry in insertion order
 *
 * Retrieves the newest entry (most recently inserted).
 *
 * @param[in] lhm Pointer to LinkedHashMap
 * @param[out] keyOut Buffer for key (can be NULL to skip)
 * @param[out] valueOut Buffer for value (can be NULL to skip)
 *
 * @return zzOpResult with status:
 *         - SUCCESS: Last entry retrieved successfully
 *         - ERROR: Possible errors:
 *           * "LinkedHashMap pointer is NULL"
 *           * "Map is empty"
 */
zzOpResult zzLinkedHashMapGetLast(const zzLinkedHashMap *lhm, void *keyOut, void *valueOut);

#endif
