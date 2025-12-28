#ifndef HASH_SET_H
#define HASH_SET_H

#include "common.h"

// ====================
// ==== HASHSET (Separate Chaining) ====
// ====================

typedef struct SetNode {
    struct SetNode *next;
    uint32_t hash;
    unsigned char key[];
} SetNode;

typedef struct {
    SetNode **buckets;
    size_t keySize;
    size_t capacity;
    size_t size;
    float loadFactor;
    zzHashFn hashFn;
    zzEqualsFn equalsFn;
    zzFreeFn keyFree;
} zzHashSet;

/**
 * Initialize a new hash set
 */
bool setInit(zzHashSet *s, size_t keySize, size_t capacity,
             zzHashFn hashFn, zzEqualsFn equalsFn, zzFreeFn keyFree);

/**
 * Free hash set and all nodes
 */
void setFree(zzHashSet *s);

/**
 * Insert a key into the set
 * @return true if inserted, false if already exists
 */
bool setInsert(zzHashSet *s, const void *key);

/**
 * Check if key exists in set
 */
bool setContains(const zzHashSet *s, const void *key);

/**
 * Remove a key from the set
 * @return true if removed, false if not found
 */
bool setRemove(zzHashSet *s, const void *key);

/**
 * Clear all elements
 */
void setClear(zzHashSet *s);

/**
 * Iterate over all keys
 */
void setForEach(zzHashSet *s, zzForEachFn fn, void *userdata);

/**
 * Union: s1 = s1 ∪ s2
 */
bool setUnion(zzHashSet *s1, const zzHashSet *s2);

/**
 * Intersection: result contains only elements in both s1 and s2
 * @param result Uninitialized zzHashSet to store result (will be initialized by this function)
 * @return true on success, false on failure
 */
bool setIntersection(const zzHashSet *s1, const zzHashSet *s2, zzHashSet *result,
                     zzHashFn hashFn, zzEqualsFn equalsFn, zzFreeFn keyFree);

/**
 * Difference: result = s1 - s2
 * @param result Uninitialized zzHashSet to store result (will be initialized by this function)
 * @return true on success, false on failure
 */
bool setDifference(const zzHashSet *s1, const zzHashSet *s2, zzHashSet *result,
                   zzHashFn hashFn, zzEqualsFn equalsFn, zzFreeFn keyFree);

#endif // HASH_SET_H
