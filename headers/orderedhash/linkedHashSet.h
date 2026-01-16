/**
 * @file linkedHashSet.h
 * @brief LinkedHashSet - Hash set with insertion order preservation
 *
 * Provides a hash set that maintains the insertion order of keys through
 * a doubly-linked list. Combines O(1) hash set performance with ordered
 * iteration capabilities. Keys can be tested for membership in O(1) time
 * and iterated in insertion order.
 */

#ifndef LINKED_HASH_SET_H
#define LINKED_HASH_SET_H

#include "types.h"
#include "utils.h"
#include "result.h"

typedef struct LHSetNode {
    struct LHSetNode *hashNext;
    struct LHSetNode *prev;
    struct LHSetNode *next;
    uint32_t hash;
    unsigned char key[];
} LHSetNode;

/**
 * @brief LinkedHashSet maintains insertion order with hash table performance
 *
 * Combines hash table with doubly-linked list to preserve insertion order.
 * Provides O(1) membership test like HashSet plus ordered iteration.
 */
typedef struct zzLinkedHashSet {
    LHSetNode **buckets;
    LHSetNode *head;
    LHSetNode *tail;
    size_t keySize;
    size_t capacity;
    size_t size;
    float loadFactor;
    zzHashFn hashFn;
    zzEqualsFn equalsFn;
    zzFreeFn keyFree;
} zzLinkedHashSet;

/**
 * @brief Initialize LinkedHashSet with specified key size and capacity
 *
 * Allocates hash table and sets up linked list structure.
 * Must be freed with zzLinkedHashSetFree when done.
 *
 * @param[out] lhs Pointer to LinkedHashSet to initialize
 * @param[in] keySize Size of each key in bytes (must be > 0)
 * @param[in] capacity Initial capacity (use 0 for default of 16)
 * @param[in] hashFn Hash function (NULL for default)
 * @param[in] equalsFn Equality function (NULL for default)
 * @param[in] keyFree Optional key cleanup function (can be NULL)
 *
 * @return zzOpResult with status:
 *         - SUCCESS: LinkedHashSet initialized successfully
 *         - ERROR: Possible errors:
 *           * "LinkedHashSet pointer is NULL"
 *           * "Key size cannot be zero"
 *           * "Failed to allocate buckets"
 *
 * @see zzLinkedHashSetFree
 */
zzOpResult zzLinkedHashSetInit(zzLinkedHashSet *lhs, size_t keySize, size_t capacity,
                       zzHashFn hashFn, zzEqualsFn equalsFn, zzFreeFn keyFree);

/**
 * @brief Free all memory associated with LinkedHashSet
 *
 * Calls keyFree on all keys (if provided), then frees internal structures.
 * The LinkedHashSet struct itself is not freed.
 *
 * @param[in,out] lhs Pointer to LinkedHashSet to free
 *
 * @note Safe to call with NULL pointer (no-op)
 */
void zzLinkedHashSetFree(zzLinkedHashSet *lhs);

/**
 * @brief Insert key into set
 *
 * Adds new key to set and appends to insertion-order list.
 * Key is only inserted if not already present (set semantics).
 * May trigger rehash if load factor exceeded.
 *
 * @param[in,out] lhs Pointer to LinkedHashSet
 * @param[in] key Pointer to key data (will be copied)
 *
 * @return zzOpResult with status:
 *         - SUCCESS: Key inserted successfully
 *         - ERROR: Possible errors:
 *           * "LinkedHashSet pointer is NULL"
 *           * "Key pointer is NULL"
 *           * "Key already exists"
 *           * "Failed to allocate node"
 *           * "Failed to rehash (out of memory)"
 */
zzOpResult zzLinkedHashSetInsert(zzLinkedHashSet *lhs, const void *key);

/**
 * @brief Check if key exists in set
 *
 * @param[in] lhs Pointer to LinkedHashSet
 * @param[in] key Pointer to key to check
 *
 * @return true if key exists, false otherwise
 */
bool zzLinkedHashSetContains(const zzLinkedHashSet *lhs, const void *key);

/**
 * @brief Remove key from set
 *
 * Removes key from both hash table and insertion-order list.
 * Calls keyFree if provided.
 *
 * @param[in,out] lhs Pointer to LinkedHashSet
 * @param[in] key Pointer to key to remove
 *
 * @return zzOpResult with status:
 *         - SUCCESS: Key removed successfully
 *         - ERROR: Possible errors:
 *           * "LinkedHashSet pointer is NULL"
 *           * "Key pointer is NULL"
 *           * "Key not found"
 */
zzOpResult zzLinkedHashSetRemove(zzLinkedHashSet *lhs, const void *key);

/**
 * @brief Clear all keys from set
 *
 * Removes all keys (calls keyFree if provided).
 * Keeps allocated buckets. Size becomes 0, capacity unchanged.
 *
 * @param[in,out] lhs Pointer to LinkedHashSet
 */
void zzLinkedHashSetClear(zzLinkedHashSet *lhs);

/**
 * @brief Get first key in insertion order
 *
 * Retrieves the oldest key (first inserted).
 *
 * @param[in] lhs Pointer to LinkedHashSet
 * @param[out] keyOut Buffer where key will be copied (must be keySize bytes)
 *
 * @return zzOpResult with status:
 *         - SUCCESS: First key retrieved successfully
 *         - ERROR: Possible errors:
 *           * "LinkedHashSet pointer is NULL"
 *           * "Key output pointer is NULL"
 *           * "Set is empty"
 */
zzOpResult zzLinkedHashSetGetFirst(const zzLinkedHashSet *lhs, void *keyOut);

/**
 * @brief Get last key in insertion order
 *
 * Retrieves the newest key (most recently inserted).
 *
 * @param[in] lhs Pointer to LinkedHashSet
 * @param[out] keyOut Buffer where key will be copied (must be keySize bytes)
 *
 * @return zzOpResult with status:
 *         - SUCCESS: Last key retrieved successfully
 *         - ERROR: Possible errors:
 *           * "LinkedHashSet pointer is NULL"
 *           * "Key output pointer is NULL"
 *           * "Set is empty"
 */
zzOpResult zzLinkedHashSetGetLast(const zzLinkedHashSet *lhs, void *keyOut);

#endif /* LINKED_HASH_SET_H */
