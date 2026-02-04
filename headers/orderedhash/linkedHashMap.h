/**
 * @file linkedHashMap.h
 * @brief Linked hash map implementation maintaining insertion order.
 *
 * This module implements a linked hash map that combines the benefits of hash maps
 * and linked lists. It provides O(1) average time complexity for lookup, insertion,
 * and deletion operations while maintaining the insertion order of entries.
 * The implementation uses a hash table for fast access combined with a doubly-linked
 * list to preserve the order of insertion.
 */

#ifndef LINKED_HASH_MAP_H
#define LINKED_HASH_MAP_H

#include "types.h"
#include "utils.h"
#include "result.h"
#include "iterator.h"

/**
 * @brief Structure representing a node in the linked hash map.
 *
 * Each node contains pointers for both the hash table collision chain and the
 * doubly-linked list that maintains insertion order, along with the hash value
 * and a flexible array member to store key and value data.
 */
typedef struct LHMapNode {
    struct LHMapNode *hashNext; /**< Pointer to the next node in the hash bucket chain */
    struct LHMapNode *prev;     /**< Pointer to the previous node in the insertion order list */
    struct LHMapNode *next;     /**< Pointer to the next node in the insertion order list */
    uint32_t hash;              /**< Hash value of the key for quick comparison */
    unsigned char data[];       /**< Flexible array member to store key and value data */
} LHMapNode;

/**
 * @brief Structure representing a linked hash map.
 *
 * This structure maintains both a hash table for fast access and a doubly-linked
 * list to preserve insertion order. It tracks the current size and capacity,
 * and provides function pointers for hashing, equality checking, and memory management.
 */
typedef struct zzLinkedHashMap {
    LHMapNode **buckets;   /**< Array of pointers to the heads of hash collision chains */
    LHMapNode *head;       /**< Pointer to the first node in insertion order */
    LHMapNode *tail;       /**< Pointer to the last node in insertion order */
    size_t capacity;       /**< Number of buckets in the hash table */
    size_t size;           /**< Current number of key-value pairs in the map */
    size_t keySize;        /**< Size in bytes of each key */
    size_t valueSize;      /**< Size in bytes of each value */
    float loadFactor;      /**< Threshold for triggering resize operations (default 0.75) */
    zzHashFn hashFn;       /**< Function to compute hash values for keys */
    zzEqualsFn equalsFn;   /**< Function to compare keys for equality */
    zzFreeFn keyFree;      /**< Function to free key memory, or NULL if not needed */
    zzFreeFn valueFree;    /**< Function to free value memory, or NULL if not needed */
} zzLinkedHashMap;

/**
 * @brief Structure representing an iterator for LinkedHashMap.
 *
 * This structure provides iteration through a LinkedHashMap in insertion order,
 * following the linked list of nodes.
 */
typedef struct zzLinkedHashMapIterator {
    zzLinkedHashMap *map;       /**< Pointer to the LinkedHashMap being iterated */
    LHMapNode *current;         /**< Pointer to the current node */
    LHMapNode *lastReturned;    /**< Pointer to the last returned node */
    zzIteratorState state;      /**< Current state of the iterator */
} zzLinkedHashMapIterator;

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
zzOpResult zzLinkedHashMapInit(zzLinkedHashMap *lhm, size_t keySize, size_t valueSize, size_t capacity, zzHashFn hashFn, zzEqualsFn equalsFn, zzFreeFn keyFree, zzFreeFn valueFree);

/**
 * @brief Frees all resources associated with the LinkedHashMap.
 *
 * This function releases all memory used by the LinkedHashMap, including calling
 * the custom free functions for each key and value if provided. After this
 * function returns, the LinkedHashMap structure should not be used until reinitialized.
 *
 * @param[in,out] lhm Pointer to the LinkedHashMap to free
 */
void zzLinkedHashMapFree(zzLinkedHashMap *lhm);

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
zzOpResult zzLinkedHashMapPut(zzLinkedHashMap *lhm, const void *key, const void *value);

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
zzOpResult zzLinkedHashMapGet(const zzLinkedHashMap *lhm, const void *key, void *valueOut);

/**
 * @brief Checks if the LinkedHashMap contains the specified key.
 *
 * This function checks whether the given key exists in the linked hash map.
 *
 * @param[in] lhm Pointer to the LinkedHashMap to check
 * @param[in] key Pointer to the key to look for
 * @return true if the key exists in the map, false otherwise
 */
bool zzLinkedHashMapContains(const zzLinkedHashMap *lhm, const void *key);

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
zzOpResult zzLinkedHashMapRemove(zzLinkedHashMap *lhm, const void *key);

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
void zzLinkedHashMapClear(zzLinkedHashMap *lhm);

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
zzOpResult zzLinkedHashMapGetFirst(const zzLinkedHashMap *lhm, void *keyOut, void *valueOut);

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
zzOpResult zzLinkedHashMapGetLast(const zzLinkedHashMap *lhm, void *keyOut, void *valueOut);

/**
 * @brief Initializes an iterator for the LinkedHashMap.
 *
 * This function initializes an iterator to traverse the LinkedHashMap in insertion order.
 * The iterator will visit key-value pairs in the order they were inserted.
 *
 * @param[out] it Pointer to the iterator structure to initialize
 * @param[in] lhm Pointer to the LinkedHashMap to iterate over
 */
void zzLinkedHashMapIteratorInit(zzLinkedHashMapIterator *it, zzLinkedHashMap *lhm);

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
bool zzLinkedHashMapIteratorNext(zzLinkedHashMapIterator *it, void *keyOut, void *valueOut);

/**
 * @brief Checks if the iterator has more elements.
 *
 * This function checks whether the iterator can advance to another key-value pair
 * without actually advancing it.
 *
 * @param[in] it Pointer to the iterator to check
 * @return true if there are more elements, false otherwise
 */
bool zzLinkedHashMapIteratorHasNext(const zzLinkedHashMapIterator *it);

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
zzOpResult zzLinkedHashMapIteratorRemove(zzLinkedHashMapIterator *it);

#endif