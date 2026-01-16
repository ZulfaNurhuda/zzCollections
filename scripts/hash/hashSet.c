/**
 * @file hashSet.c
 * @brief Hash set implementation with separate chaining for collision resolution
 *
 * Source implementation for the hash set data structure that provides
 * O(1) average time complexity for insert, lookup, and delete operations.
 * Uses separate chaining with linked lists to handle hash collisions.
 */

#include "hashSet.h"
#include <string.h>
#include <stdlib.h>

/**
 * @brief Initialize a hash set with specified parameters
 *
 * Sets up the internal structure of the hash set including allocating the bucket array
 * and initializing all fields. If hashFn or equalsFn are NULL, default implementations
 * are used.
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
 *           * "Failed to allocate buckets"
 *
 * @see zzHashSetFree
 */
zzOpResult zzHashSetInit(zzHashSet *s, size_t keySize, size_t capacity,
             zzHashFn hashFn, zzEqualsFn equalsFn, zzFreeFn keyFree) {
    if (!s) return ZZ_ERR("HashSet pointer is NULL");
    if (keySize == 0) return ZZ_ERR("Key size cannot be zero");
    if (capacity == 0) capacity = 16;

    s->buckets = calloc(capacity, sizeof(SetNode*));
    if (!s->buckets) return ZZ_ERR("Failed to allocate buckets");

    s->keySize = keySize;
    s->capacity = capacity;
    s->size = 0;
    s->loadFactor = 0.75f;
    s->hashFn = hashFn ? hashFn : zzDefaultHash;
    s->equalsFn = equalsFn ? equalsFn : zzDefaultEquals;
    s->keyFree = keyFree;
    return ZZ_OK();
}

/**
 * @brief Free all memory associated with the hash set
 *
 * Iterates through all buckets and nodes, calling the keyFree callback
 * if provided during initialization. Then frees all allocated memory for nodes and buckets.
 * The hash set struct itself is not freed (caller owns it).
 *
 * @param[in,out] s Pointer to the hash set to free
 *
 * @note Safe to call with NULL pointer (no-op)
 */
void zzHashSetFree(zzHashSet *s) {
    if (!s || !s->buckets) return;

    for (size_t i = 0; i < s->capacity; i++) {
        SetNode *cur = s->buckets[i];
        while (cur) {
            SetNode *next = cur->next;
            if (s->keyFree) s->keyFree(cur->key);
            free(cur);
            cur = next;
        }
    }
    free(s->buckets);
    s->buckets = NULL;
    s->size = 0;
}

/**
 * @brief Internal function to resize the hash set when load factor threshold is exceeded
 *
 * Doubles the capacity of the hash set and redistributes all existing entries
 * to their new positions based on the new capacity. This maintains O(1) average
 * time complexity by preventing the load factor from becoming too high.
 *
 * @param[in,out] s Pointer to the hash set to rehash
 *
 * @return zzOpResult with status:
 *         - SUCCESS: Rehash completed successfully
 *         - ERROR: "Failed to rehash (out of memory)" if allocation fails
 *
 * @note This is an internal function and should not be called directly
 */
static zzOpResult zzHashSetRehash(zzHashSet *s) {
    size_t newCap = s->capacity * 2;
    SetNode **newBuckets = calloc(newCap, sizeof(SetNode*));
    if (!newBuckets) return ZZ_ERR("Failed to rehash (out of memory)");

    for (size_t i = 0; i < s->capacity; i++) {
        SetNode *cur = s->buckets[i];
        while (cur) {
            SetNode *next = cur->next;
            size_t idx = cur->hash % newCap;
            cur->next = newBuckets[idx];
            newBuckets[idx] = cur;
            cur = next;
        }
    }

    free(s->buckets);
    s->buckets = newBuckets;
    s->capacity = newCap;
    return ZZ_OK();
}

/**
 * @brief Insert a key into the hash set
 *
 * Computes the hash of the key and inserts it into the appropriate bucket.
 * Returns an error if the key already exists in the set.
 * Triggers rehashing if the load factor exceeds the threshold.
 *
 * @param[in,out] s Pointer to the hash set
 * @param[in] key Pointer to the key data to insert
 *
 * @return zzOpResult with status:
 *         - SUCCESS: Key inserted successfully
 *         - ERROR: Possible errors:
 *           * "HashSet pointer is NULL"
 *           * "Key pointer is NULL"
 *           * "Key already exists"
 *           * "Failed to allocate node"
 *
 * @see zzHashSetContains
 * @see zzHashSetRemove
 */
zzOpResult zzHashSetInsert(zzHashSet *s, const void *key) {
    if (!s) return ZZ_ERR("HashSet pointer is NULL");
    if (!key) return ZZ_ERR("Key pointer is NULL");

    uint32_t hash = s->hashFn(key);
    size_t idx = hash % s->capacity;

    SetNode *cur = s->buckets[idx];
    while (cur) {
        if (cur->hash == hash && s->equalsFn(cur->key, key))
            return ZZ_ERR("Key already exists");
        cur = cur->next;
    }

    SetNode *node = malloc(sizeof(SetNode) + s->keySize);
    if (!node) return ZZ_ERR("Failed to allocate node");

    node->hash = hash;
    memcpy(node->key, key, s->keySize);
    node->next = s->buckets[idx];
    s->buckets[idx] = node;
    s->size++;

    if ((float)s->size / s->capacity > s->loadFactor) {
        zzOpResult rehashResult = zzHashSetRehash(s);
        if (ZZ_IS_ERR(rehashResult)) return rehashResult;
    }

    return ZZ_OK();
}

/**
 * @brief Check if a key exists in the hash set
 *
 * Determines whether the specified key exists in the hash set.
 * Uses the hash function to determine the bucket and the equals function to check for key
 * equality in case of collisions.
 *
 * @param[in] s Pointer to the hash set
 * @param[in] key Pointer to the key to check
 *
 * @return true if key exists, false otherwise
 */
bool zzHashSetContains(const zzHashSet *s, const void *key) {
    if (!s || !key) return false;

    uint32_t hash = s->hashFn(key);
    size_t idx = hash % s->capacity;

    SetNode *cur = s->buckets[idx];
    while (cur) {
        if (cur->hash == hash && s->equalsFn(cur->key, key))
            return true;
        cur = cur->next;
    }
    return false;
}

/**
 * @brief Remove a key from the hash set
 *
 * Searches for the specified key in the hash set and removes the corresponding entry.
 * Calls the keyFree callback if provided during initialization to clean up
 * the key data before freeing the node memory.
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
zzOpResult zzHashSetRemove(zzHashSet *s, const void *key) {
    if (!s) return ZZ_ERR("HashSet pointer is NULL");
    if (!key) return ZZ_ERR("Key pointer is NULL");

    uint32_t hash = s->hashFn(key);
    size_t idx = hash % s->capacity;

    SetNode **cur = &s->buckets[idx];
    while (*cur) {
        SetNode *node = *cur;
        if (node->hash == hash && s->equalsFn(node->key, key)) {
            *cur = node->next;
            if (s->keyFree) s->keyFree(node->key);
            free(node);
            s->size--;
            return ZZ_OK();
        }
        cur = &node->next;
    }
    return ZZ_ERR("Key not found");
}

/**
 * @brief Remove all keys from the hash set
 *
 * Empties the hash set by removing all entries and calling the keyFree
 * callback if provided during initialization. The bucket array is preserved but emptied.
 *
 * @param[in,out] s Pointer to the hash set to clear
 *
 * @note Safe to call with NULL pointer (no-op)
 */
void zzHashSetClear(zzHashSet *s) {
    if (!s) return;

    for (size_t i = 0; i < s->capacity; i++) {
        SetNode *cur = s->buckets[i];
        while (cur) {
            SetNode *next = cur->next;
            if (s->keyFree) s->keyFree(cur->key);
            free(cur);
            cur = next;
        }
        s->buckets[i] = NULL;
    }
    s->size = 0;
}

/**
 * @brief Iterate over all keys in the hash set and apply a function to each
 *
 * Calls the provided function for each key in the hash set, passing along
 * user-defined data. This allows for operations like printing, counting,
 * or transforming elements without exposing the internal structure.
 *
 * @param[in] s Pointer to the hash set to iterate
 * @param[in] fn Function to call for each key
 * @param[in,out] userdata Pointer to user-defined data to pass to the function
 *
 * @note Safe to call with NULL pointers (no-op)
 */
void zzHashSetForEach(zzHashSet *s, zzForEachFn fn, void *userdata) {
    if (!s || !fn) return;

    for (size_t i = 0; i < s->capacity; i++) {
        SetNode *cur = s->buckets[i];
        while (cur) {
            fn(cur->key, userdata);
            cur = cur->next;
        }
    }
}

/**
 * @brief Compute the union of two hash sets (adds all elements from s2 to s1)
 *
 * Modifies the first set (s1) to contain all elements that are in either s1 or s2.
 * Since insertion ignores duplicate keys, this effectively computes the union.
 *
 * @param[in,out] s1 Pointer to the first hash set (will be modified to contain union)
 * @param[in] s2 Pointer to the second hash set (remains unchanged)
 *
 * @return zzOpResult with status:
 *         - SUCCESS: Union computed successfully
 *         - ERROR: Possible errors:
 *           * "HashSet s1 pointer is NULL"
 *           * "HashSet s2 pointer is NULL"
 *           * Errors from zzHashSetInsert (e.g., allocation failure)
 *
 * @note Keys that already exist in s1 will not cause an error
 */
zzOpResult zzHashSetUnion(zzHashSet *s1, const zzHashSet *s2) {
    if (!s1) return ZZ_ERR("HashSet s1 pointer is NULL");
    if (!s2) return ZZ_ERR("HashSet s2 pointer is NULL");

    for (size_t i = 0; i < s2->capacity; i++) {
        SetNode *cur = s2->buckets[i];
        while (cur) {
            zzOpResult insertResult = zzHashSetInsert(s1, cur->key);
            if (ZZ_IS_ERR(insertResult) && insertResult.error != NULL) {
                // Ignore "Key already exists" error for union
                if (insertResult.error[0] != 'K') { // Not "Key already exists"
                    return insertResult;
                }
            }
            cur = cur->next;
        }
    }
    return ZZ_OK();
}

/**
 * @brief Compute the intersection of two hash sets (elements in both s1 and s2)
 *
 * Creates a new hash set containing only the elements that exist in both input sets.
 * The result set must be initialized separately and will be cleared before populating.
 *
 * @param[in] s1 Pointer to the first hash set
 * @param[in] s2 Pointer to the second hash set
 * @param[out] result Pointer to the result hash set (will be overwritten)
 * @param[in] hashFn Hash function for the result set
 * @param[in] equalsFn Equality function for the result set
 * @param[in] keyFree Optional cleanup function for keys in the result set
 *
 * @return zzOpResult with status:
 *         - SUCCESS: Intersection computed successfully
 *         - ERROR: Possible errors:
 *           * "HashSet s1 pointer is NULL"
 *           * "HashSet s2 pointer is NULL"
 *           * "Result HashSet pointer is NULL"
 *           * Errors from zzHashSetInit or zzHashSetInsert
 *
 * @note The result set will be initialized and populated with intersecting elements
 * @see zzHashSetInit
 */
zzOpResult zzHashSetIntersection(const zzHashSet *s1, const zzHashSet *s2, zzHashSet *result,
                     zzHashFn hashFn, zzEqualsFn equalsFn, zzFreeFn keyFree) {
    if (!s1) return ZZ_ERR("HashSet s1 pointer is NULL");
    if (!s2) return ZZ_ERR("HashSet s2 pointer is NULL");
    if (!result) return ZZ_ERR("Result HashSet pointer is NULL");

    zzOpResult initResult = zzHashSetInit(result, s1->keySize, 16, hashFn, equalsFn, keyFree);
    if (ZZ_IS_ERR(initResult)) return initResult;

    for (size_t i = 0; i < s1->capacity; i++) {
        SetNode *cur = s1->buckets[i];
        while (cur) {
            if (zzHashSetContains(s2, cur->key)) {
                zzOpResult insertResult = zzHashSetInsert(result, cur->key);
                if (ZZ_IS_ERR(insertResult)) {
                    zzHashSetFree(result);
                    return insertResult;
                }
            }
            cur = cur->next;
        }
    }

    return ZZ_OK();
}

/**
 * @brief Compute the difference of two hash sets (elements in s1 but not in s2)
 *
 * Creates a new hash set containing only the elements that exist in the first set
 * but not in the second set (s1 - s2). The result set must be initialized separately
 * and will be cleared before populating.
 *
 * @param[in] s1 Pointer to the first hash set (minuend)
 * @param[in] s2 Pointer to the second hash set (subtrahend)
 * @param[out] result Pointer to the result hash set (will be overwritten)
 * @param[in] hashFn Hash function for the result set
 * @param[in] equalsFn Equality function for the result set
 * @param[in] keyFree Optional cleanup function for keys in the result set
 *
 * @return zzOpResult with status:
 *         - SUCCESS: Difference computed successfully
 *         - ERROR: Possible errors:
 *           * "HashSet s1 pointer is NULL"
 *           * "HashSet s2 pointer is NULL"
 *           * "Result HashSet pointer is NULL"
 *           * Errors from zzHashSetInit or zzHashSetInsert
 *
 * @note The result set will be initialized and populated with difference elements
 * @see zzHashSetInit
 */
zzOpResult zzHashSetDifference(const zzHashSet *s1, const zzHashSet *s2, zzHashSet *result,
                   zzHashFn hashFn, zzEqualsFn equalsFn, zzFreeFn keyFree) {
    if (!s1) return ZZ_ERR("HashSet s1 pointer is NULL");
    if (!s2) return ZZ_ERR("HashSet s2 pointer is NULL");
    if (!result) return ZZ_ERR("Result HashSet pointer is NULL");

    zzOpResult initResult = zzHashSetInit(result, s1->keySize, 16, hashFn, equalsFn, keyFree);
    if (ZZ_IS_ERR(initResult)) return initResult;

    for (size_t i = 0; i < s1->capacity; i++) {
        SetNode *cur = s1->buckets[i];
        while (cur) {
            if (!zzHashSetContains(s2, cur->key)) {
                zzOpResult insertResult = zzHashSetInsert(result, cur->key);
                if (ZZ_IS_ERR(insertResult)) {
                    zzHashSetFree(result);
                    return insertResult;
                }
            }
            cur = cur->next;
        }
    }

    return ZZ_OK();
}
