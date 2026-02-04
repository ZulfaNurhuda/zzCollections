/**
 * @file hashSet.h
 * @brief Hash set implementation using chaining for collision resolution.
 *
 * This module implements a hash set that stores unique keys using a hash table
 * with chaining for collision resolution. The implementation provides average O(1)
 * time complexity for insertion, lookup, and deletion operations. The hash set
 * supports generic key types and includes automatic resizing when the load factor
 * threshold is exceeded.
 */

#ifndef HASH_SET_H
#define HASH_SET_H

#include "types.h"
#include "utils.h"
#include "result.h"
#include "iterator.h"

/**
 * @brief Structure representing a node in the hash set's collision chain.
 *
 * Each node contains a pointer to the next node in the chain, the hash value
 * of the key, and a flexible array member to store the key data.
 */
typedef struct SetNode {
    struct SetNode *next; /**< Pointer to the next node in the collision chain */
    uint32_t hash;        /**< Hash value of the key for quick comparison */
    unsigned char key[];  /**< Flexible array member to store the key data */
} SetNode;

/**
 * @brief Structure representing a hash set.
 *
 * This structure maintains an array of buckets (collision chains), tracks the
 * current size and capacity, and provides function pointers for hashing,
 * equality checking, and memory management.
 */
typedef struct zzHashSet {
    SetNode **buckets;   /**< Array of pointers to the heads of collision chains */
    size_t capacity;     /**< Number of buckets in the hash table */
    size_t size;         /**< Current number of keys in the set */
    size_t keySize;      /**< Size in bytes of each key */
    float loadFactor;    /**< Threshold for triggering resize operations (default 0.75) */
    zzHashFn hashFn;     /**< Function to compute hash values for keys */
    zzEqualsFn equalsFn; /**< Function to compare keys for equality */
    zzFreeFn keyFree;    /**< Function to free key memory, or NULL if not needed */
} zzHashSet;

/**
 * @brief Structure representing an iterator for HashSet.
 *
 * This structure provides iteration through a HashSet,
 * maintaining the current bucket and node position.
 */
typedef struct zzHashSetIterator {
    zzHashSet *set;        /**< Pointer to the HashSet being iterated */
    size_t bucketIndex;    /**< Current bucket index */
    SetNode *currentNode;  /**< Current node in the bucket chain */
    SetNode *lastReturned; /**< Pointer to the last returned node */
    zzIteratorState state; /**< Current state of the iterator */
} zzHashSetIterator;

/**
 * @brief Initializes a new HashSet with the specified key size.
 *
 * This function initializes a HashSet structure with the given key size,
 * initial capacity, and custom functions for hashing, equality checking, and memory
 * management. The hash set will be empty after initialization.
 *
 * @param[out] s Pointer to the HashSet structure to initialize
 * @param[in] keySize Size in bytes of each key that will be stored in the set
 * @param[in] capacity Initial capacity of the hash set (will be adjusted to at least 16)
 * @param[in] hashFn Function to compute hash values for keys, or NULL to use default
 * @param[in] equalsFn Function to compare keys for equality, or NULL to use default
 * @param[in] keyFree Function to free key memory when entries are removed or the set is freed, or NULL if not needed
 * @return zzOpResult with status ZZ_SUCCESS on success, or ZZ_ERROR with error message on failure
 */
zzOpResult zzHashSetInit(zzHashSet *s, size_t keySize, size_t capacity, zzHashFn hashFn, zzEqualsFn equalsFn, zzFreeFn keyFree);

/**
 * @brief Frees all resources associated with the HashSet.
 *
 * This function releases all memory used by the HashSet, including calling
 * the custom free function for each key if provided. After this function
 * returns, the HashSet structure should not be used until reinitialized.
 *
 * @param[in,out] s Pointer to the HashSet to free
 */
void zzHashSetFree(zzHashSet *s);

/**
 * @brief Inserts a key into the HashSet.
 *
 * This function inserts a new key into the hash set if it doesn't already exist.
 * The key is copied into the set's internal storage. If the load factor threshold
 * is exceeded after the insertion, the hash set will be automatically resized.
 *
 * @param[in,out] s Pointer to the HashSet to insert into
 * @param[in] key Pointer to the key to insert (contents will be copied)
 * @return zzOpResult with status ZZ_SUCCESS on success, or ZZ_ERROR with error message on failure
 */
zzOpResult zzHashSetInsert(zzHashSet *s, const void *key);

/**
 * @brief Checks if the HashSet contains the specified key.
 *
 * This function checks whether the given key exists in the hash set.
 *
 * @param[in] s Pointer to the HashSet to check
 * @param[in] key Pointer to the key to look for
 * @return true if the key exists in the set, false otherwise
 */
bool zzHashSetContains(const zzHashSet *s, const void *key);

/**
 * @brief Removes a key from the HashSet.
 *
 * This function removes the specified key from the hash set if it exists.
 * If a custom free function was provided for keys, it will be called on
 * the removed key. The function returns an error if the key is not present
 * in the set.
 *
 * @param[in,out] s Pointer to the HashSet to remove from
 * @param[in] key Pointer to the key to remove
 * @return zzOpResult with status ZZ_SUCCESS on success, or ZZ_ERROR with error message on failure
 */
zzOpResult zzHashSetRemove(zzHashSet *s, const void *key);

/**
 * @brief Clears all keys from the HashSet.
 *
 * This function removes all keys from the hash set by calling the custom free
 * function for each key (if provided) and deallocating all nodes. The hash set
 * becomes empty after this operation.
 *
 * @param[in,out] s Pointer to the HashSet to clear
 */
void zzHashSetClear(zzHashSet *s);

/**
 * @brief Applies a function to each element in the HashSet.
 *
 * This function iterates through all elements in the hash set and applies
 * the provided function to each element, passing along user-defined data.
 *
 * @param[in,out] s Pointer to the HashSet to iterate over
 * @param[in] fn Function to apply to each element
 * @param[in] userdata Pointer to user-defined data to pass to the function
 */
void zzHashSetForEach(zzHashSet *s, zzForEachFn fn, void *userdata);

/**
 * @brief Performs a union operation between two hash sets.
 *
 * This function adds all elements from the second hash set to the first hash set.
 * After the operation, the first set will contain all elements that were in either set.
 *
 * @param[in,out] s1 Pointer to the first HashSet (destination for the union)
 * @param[in] s2 Pointer to the second HashSet (source for the union)
 * @return zzOpResult with status ZZ_SUCCESS on success, or ZZ_ERROR with error message on failure
 */
zzOpResult zzHashSetUnion(zzHashSet *s1, const zzHashSet *s2);

/**
 * @brief Performs an intersection operation between two hash sets.
 *
 * This function creates a new hash set containing only the elements that are
 * present in both input sets. The result set must be initialized before calling
 * this function.
 *
 * @param[in] s1 Pointer to the first HashSet
 * @param[in] s2 Pointer to the second HashSet
 * @param[out] result Pointer to the HashSet where the intersection result will be stored
 * @param[in] hashFn Function to compute hash values for keys in the result set
 * @param[in] equalsFn Function to compare keys for equality in the result set
 * @param[in] keyFree Function to free key memory in the result set
 * @return zzOpResult with status ZZ_SUCCESS on success, or ZZ_ERROR with error message on failure
 */
zzOpResult zzHashSetIntersection(const zzHashSet *s1, const zzHashSet *s2, zzHashSet *result, zzHashFn hashFn, zzEqualsFn equalsFn, zzFreeFn keyFree);

/**
 * @brief Performs a difference operation between two hash sets.
 *
 * This function creates a new hash set containing only the elements that are
 * present in the first set but not in the second set. The result set must be
 * initialized before calling this function.
 *
 * @param[in] s1 Pointer to the first HashSet (minuend)
 * @param[in] s2 Pointer to the second HashSet (subtrahend)
 * @param[out] result Pointer to the HashSet where the difference result will be stored
 * @param[in] hashFn Function to compute hash values for keys in the result set
 * @param[in] equalsFn Function to compare keys for equality in the result set
 * @param[in] keyFree Function to free key memory in the result set
 * @return zzOpResult with status ZZ_SUCCESS on success, or ZZ_ERROR with error message on failure
 */
zzOpResult zzHashSetDifference(const zzHashSet *s1, const zzHashSet *s2, zzHashSet *result, zzHashFn hashFn, zzEqualsFn equalsFn, zzFreeFn keyFree);

/**
 * @brief Initializes an iterator for the HashSet.
 *
 * This function initializes an iterator to traverse the HashSet.
 * The iteration order is not guaranteed to be consistent between
 * different runs or after set modifications.
 *
 * @param[out] it Pointer to the iterator structure to initialize
 * @param[in] s Pointer to the HashSet to iterate over
 */
void zzHashSetIteratorInit(zzHashSetIterator *it, zzHashSet *s);

/**
 * @brief Advances the iterator to the next key.
 *
 * This function moves the iterator to the next key in the HashSet
 * and copies the current key to the output buffer. Returns false when
 * the iterator reaches the end of the set.
 *
 * @param[in,out] it Pointer to the iterator to advance
 * @param[out] keyOut Pointer to a buffer where the current key will be copied
 * @return true if a key was retrieved, false if the iterator reached the end
 */
bool zzHashSetIteratorNext(zzHashSetIterator *it, void *keyOut);

/**
 * @brief Checks if the iterator has more elements.
 *
 * This function checks whether the iterator can advance to another key
 * without actually advancing it.
 *
 * @param[in] it Pointer to the iterator to check
 * @return true if there are more elements, false otherwise
 */
bool zzHashSetIteratorHasNext(const zzHashSetIterator *it);

/**
 * @brief Removes the last key returned by the iterator.
 *
 * This function removes the key that was most recently returned by
 * zzHashSetIteratorNext. After removal, the iterator remains valid and
 * continues to the next element on the next call to Next.
 *
 * @param[in,out] it Pointer to the iterator
 * @return zzOpResult with status ZZ_SUCCESS on success, or ZZ_ERROR with error message on failure
 */
zzOpResult zzHashSetIteratorRemove(zzHashSetIterator *it);

#endif