/**
 * @file hashSet.h
 * @brief Hash set implementation for storing unique keys
 *
 * Provides O(1) average time complexity for insert, lookup, and delete operations.
 * Uses separate chaining with linked lists to handle hash collisions.
 */

#ifndef HASH_SET_H
#define HASH_SET_H

#include "types.h"
#include "utils.h"
#include "result.h"

/**
 * @brief Internal node structure for hash set entries
 *
 * Each node contains a pointer to the next node for collision resolution,
 * the cached hash value, and flexible array member for key data.
 */
typedef struct SetNode {
    struct SetNode *next;    /**< Next node in collision chain */
    uint32_t hash;          /**< Cached hash value for efficiency */
    unsigned char key[];    /**< Flexible array for key storage */
} SetNode;

/**
 * @brief Hash set implementation with separate chaining for collision resolution
 *
 * Stores unique keys with O(1) average time complexity for operations.
 * Automatically resizes when load factor threshold is exceeded.
 */
typedef struct zzHashSet {
    SetNode **buckets;      /**< Array of bucket pointers */
    size_t keySize;         /**< Size of each key in bytes */
    size_t capacity;        /**< Number of buckets */
    size_t size;           /**< Current number of keys */
    float loadFactor;      /**< Load factor threshold for resizing */
    zzHashFn hashFn;       /**< Hash function for keys */
    zzEqualsFn equalsFn;   /**< Equality function for keys */
    zzFreeFn keyFree;      /**< Optional cleanup function for keys */
} zzHashSet;

/**
 * @brief Initialize a hash set with specified parameters
 *
 * @param[out] s Pointer to the hash set to initialize
 * @param[in] keySize Size of each key in bytes (must be > 0)
 * @param[in] capacity Initial number of buckets (use 0 for default of 16)
 * @param[in] hashFn Hash function for keys
 * @param[in] equalsFn Equality function for key comparison
 * @param[in] keyFree Optional cleanup function for keys (can be NULL)
 *
 * @return zzOpResult with status:
 *         - SUCCESS: Hash set initialized successfully
 *         - ERROR: Possible errors:
 *           * "HashSet pointer is NULL"
 *           * "Key size cannot be zero"
 *           * "Hash function is NULL"
 *           * "Equals function is NULL"
 *           * "Failed to allocate buckets"
 *
 * @see zzHashSetFree
 */
zzOpResult zzHashSetInit(zzHashSet *s, size_t keySize, size_t capacity,
             zzHashFn hashFn, zzEqualsFn equalsFn, zzFreeFn keyFree);

/**
 * @brief Free all memory associated with the hash set
 *
 * Calls cleanup function for keys if provided during initialization.
 * The hash set struct itself is not freed (caller owns it).
 *
 * @param[in,out] s Pointer to the hash set to free
 *
 * @note Safe to call with NULL pointer (no-op)
 */
void zzHashSetFree(zzHashSet *s);

/**
 * @brief Insert a key into the set
 *
 * If the key already exists, no action is taken (sets contain unique keys).
 *
 * @param[in,out] s Pointer to the hash set
 * @param[in] key Pointer to the key data to insert
 *
 * @return zzOpResult with status:
 *         - SUCCESS: Key inserted successfully (or already exists)
 *         - ERROR: Possible errors:
 *           * "HashSet pointer is NULL"
 *           * "Key pointer is NULL"
 *           * "Failed to allocate node"
 *
 * @see zzHashSetRemove
 * @see zzHashSetContains
 */
zzOpResult zzHashSetInsert(zzHashSet *s, const void *key);

/**
 * @brief Check if a key exists in the set
 *
 * @param[in] s Pointer to the hash set
 * @param[in] key Pointer to the key to search for
 *
 * @return true if key exists, false otherwise
 */
bool zzHashSetContains(const zzHashSet *s, const void *key);

/**
 * @brief Remove a key from the set
 *
 * Calls cleanup function for the key if provided during initialization.
 *
 * @param[in,out] s Pointer to the hash set
 * @param[in] key Pointer to the key to remove
 *
 * @return zzOpResult with status:
 *         - SUCCESS: Key removed successfully
 *         - ERROR: Possible errors:
 *           * "HashSet pointer is NULL"
 *           * "Key pointer is NULL"
 *           * "Key not found"
 *
 * @see zzHashSetInsert
 */
zzOpResult zzHashSetRemove(zzHashSet *s, const void *key);

/**
 * @brief Remove all keys from the set
 *
 * Calls cleanup function for all keys if provided during initialization.
 *
 * @param[in,out] s Pointer to the hash set to clear
 *
 * @note Safe to call with NULL pointer (no-op)
 */
void zzHashSetClear(zzHashSet *s);

/**
 * @brief Iterate over all keys in the set
 *
 * Calls the provided function for each key in the set.
 * Order of iteration is not guaranteed.
 *
 * @param[in] s Pointer to the hash set
 * @param[in] fn Callback function to invoke for each key
 * @param[in,out] userdata User-provided context data passed to callback
 */
void zzHashSetForEach(zzHashSet *s, zzForEachFn fn, void *userdata);

/**
 * @brief Compute union of two sets (s1 = s1 ∪ s2)
 *
 * Adds all keys from s2 into s1. Keys already in s1 are not duplicated.
 *
 * @param[in,out] s1 Destination set (will be modified)
 * @param[in] s2 Source set (not modified)
 *
 * @return zzOpResult with status:
 *         - SUCCESS: Union computed successfully
 *         - ERROR: Possible errors:
 *           * "HashSet pointer is NULL"
 *           * "Failed to insert key"
 *
 * @see zzHashSetIntersection
 * @see zzHashSetDifference
 */
zzOpResult zzHashSetUnion(zzHashSet *s1, const zzHashSet *s2);

/**
 * @brief Compute intersection of two sets (result = s1 ∩ s2)
 *
 * Creates a new set containing only keys present in both s1 and s2.
 *
 * @param[in] s1 First set
 * @param[in] s2 Second set
 * @param[out] result Pointer to uninitialized set to store result
 * @param[in] hashFn Hash function for result set
 * @param[in] equalsFn Equality function for result set
 * @param[in] keyFree Optional cleanup function for result set keys
 *
 * @return zzOpResult with status:
 *         - SUCCESS: Intersection computed successfully
 *         - ERROR: Possible errors:
 *           * "HashSet pointer is NULL"
 *           * "Failed to initialize result set"
 *           * "Failed to insert key"
 *
 * @note Caller must call zzHashSetFree on result when done
 *
 * @see zzHashSetUnion
 * @see zzHashSetDifference
 */
zzOpResult zzHashSetIntersection(const zzHashSet *s1, const zzHashSet *s2, zzHashSet *result,
                     zzHashFn hashFn, zzEqualsFn equalsFn, zzFreeFn keyFree);

/**
 * @brief Compute difference of two sets (result = s1 - s2)
 *
 * Creates a new set containing keys in s1 that are not in s2.
 *
 * @param[in] s1 First set
 * @param[in] s2 Second set
 * @param[out] result Pointer to uninitialized set to store result
 * @param[in] hashFn Hash function for result set
 * @param[in] equalsFn Equality function for result set
 * @param[in] keyFree Optional cleanup function for result set keys
 *
 * @return zzOpResult with status:
 *         - SUCCESS: Difference computed successfully
 *         - ERROR: Possible errors:
 *           * "HashSet pointer is NULL"
 *           * "Failed to initialize result set"
 *           * "Failed to insert key"
 *
 * @note Caller must call zzHashSetFree on result when done
 *
 * @see zzHashSetUnion
 * @see zzHashSetIntersection
 */
zzOpResult zzHashSetDifference(const zzHashSet *s1, const zzHashSet *s2, zzHashSet *result,
                   zzHashFn hashFn, zzEqualsFn equalsFn, zzFreeFn keyFree);

#endif /* HASH_SET_H */
