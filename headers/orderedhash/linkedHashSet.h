/**
 * @file linkedHashSet.h
 * @brief Linked hash set implementation maintaining insertion order.
 *
 * This module implements a linked hash set that combines the benefits of hash sets
 * and linked lists. It provides O(1) average time complexity for insertion,
 * lookup, and deletion operations while maintaining the insertion order of keys.
 * The implementation uses a hash table for fast access combined with a doubly-linked
 * list to preserve the order of insertion.
 */

#ifndef LINKED_HASH_SET_H
#define LINKED_HASH_SET_H

#include "types.h"
#include "utils.h"
#include "result.h"
#include "iterator.h"

/**
 * @brief Structure representing a node in the linked hash set.
 *
 * Each node contains pointers for both the hash table collision chain and the
 * doubly-linked list that maintains insertion order, along with the hash value
 * and a flexible array member to store the key data.
 */
typedef struct LHSetNode {
    struct LHSetNode *hashNext; /**< Pointer to the next node in the hash bucket chain */
    struct LHSetNode *prev;     /**< Pointer to the previous node in the insertion order list */
    struct LHSetNode *next;     /**< Pointer to the next node in the insertion order list */
    uint32_t hash;              /**< Hash value of the key for quick comparison */
    unsigned char key[];         /**< Flexible array member to store the key data */
} LHSetNode;

/**
 * @brief Structure representing a linked hash set.
 *
 * This structure maintains both a hash table for fast access and a doubly-linked
 * list to preserve insertion order. It tracks the current size and capacity,
 * and provides function pointers for hashing, equality checking, and memory management.
 */
typedef struct zzLinkedHashSet {
    LHSetNode **buckets;   /**< Array of pointers to the heads of hash collision chains */
    LHSetNode *head;       /**< Pointer to the first node in insertion order */
    LHSetNode *tail;       /**< Pointer to the last node in insertion order */
    size_t capacity;       /**< Number of buckets in the hash table */
    size_t size;           /**< Current number of keys in the set */
    size_t keySize;        /**< Size in bytes of each key */
    float loadFactor;      /**< Threshold for triggering resize operations (default 0.75) */
    zzHashFn hashFn;       /**< Function to compute hash values for keys */
    zzEqualsFn equalsFn;   /**< Function to compare keys for equality */
    zzFreeFn keyFree;      /**< Function to free key memory, or NULL if not needed */
} zzLinkedHashSet;

/**
 * @brief Structure representing an iterator for LinkedHashSet.
 *
 * This structure provides iteration through a LinkedHashSet in insertion order,
 * following the linked list of nodes.
 */
typedef struct zzLinkedHashSetIterator {
    const zzLinkedHashSet *set; /**< Pointer to the LinkedHashSet being iterated */
    LHSetNode *current;         /**< Pointer to the current node */
    zzIteratorState state;      /**< Current state of the iterator */
} zzLinkedHashSetIterator;

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
zzOpResult zzLinkedHashSetInit(zzLinkedHashSet *lhs, size_t keySize, size_t capacity, zzHashFn hashFn, zzEqualsFn equalsFn, zzFreeFn keyFree);

/**
 * @brief Frees all resources associated with the LinkedHashSet.
 *
 * This function releases all memory used by the LinkedHashSet, including calling
 * the custom free function for each key if provided. After this function
 * returns, the LinkedHashSet structure should not be used until reinitialized.
 *
 * @param[in,out] lhs Pointer to the LinkedHashSet to free
 */
void zzLinkedHashSetFree(zzLinkedHashSet *lhs);

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
zzOpResult zzLinkedHashSetInsert(zzLinkedHashSet *lhs, const void *key);

/**
 * @brief Checks if the LinkedHashSet contains the specified key.
 *
 * This function checks whether the given key exists in the linked hash set.
 *
 * @param[in] lhs Pointer to the LinkedHashSet to check
 * @param[in] key Pointer to the key to look for
 * @return true if the key exists in the set, false otherwise
 */
bool zzLinkedHashSetContains(const zzLinkedHashSet *lhs, const void *key);

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
zzOpResult zzLinkedHashSetRemove(zzLinkedHashSet *lhs, const void *key);

/**
 * @brief Clears all keys from the LinkedHashSet.
 *
 * This function removes all keys from the linked hash set by calling the custom free
 * function for each key (if provided) and deallocating all nodes. The linked hash set
 * becomes empty after this operation while maintaining its structure.
 *
 * @param[in,out] lhs Pointer to the LinkedHashSet to clear
 */
void zzLinkedHashSetClear(zzLinkedHashSet *lhs);

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
zzOpResult zzLinkedHashSetGetFirst(const zzLinkedHashSet *lhs, void *keyOut);

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
zzOpResult zzLinkedHashSetGetLast(const zzLinkedHashSet *lhs, void *keyOut);

/**
 * @brief Initializes an iterator for the LinkedHashSet.
 *
 * This function initializes an iterator to traverse the LinkedHashSet in insertion order.
 * The iterator will visit keys in the order they were inserted.
 *
 * @param[out] it Pointer to the iterator structure to initialize
 * @param[in] lhs Pointer to the LinkedHashSet to iterate over
 */
void zzLinkedHashSetIteratorInit(zzLinkedHashSetIterator *it, const zzLinkedHashSet *lhs);

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
bool zzLinkedHashSetIteratorNext(zzLinkedHashSetIterator *it, void *keyOut);

/**
 * @brief Checks if the iterator has more elements.
 *
 * This function checks whether the iterator can advance to another key
 * without actually advancing it.
 *
 * @param[in] it Pointer to the iterator to check
 * @return true if there are more elements, false otherwise
 */
bool zzLinkedHashSetIteratorHasNext(const zzLinkedHashSetIterator *it);

#endif