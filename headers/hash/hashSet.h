#ifndef HASH_SET_H
#define HASH_SET_H

#include "common.h"
#include "result.h"

typedef struct SetNode {
    struct SetNode *next;
    uint32_t hash;
    unsigned char key[];
} SetNode;

/** @brief Hash set with separate chaining */
typedef struct zzHashSet {
    SetNode **buckets;
    size_t keySize;
    size_t capacity;
    size_t size;
    float loadFactor;
    zzHashFn hashFn;
    zzEqualsFn equalsFn;
    zzFreeFn keyFree;
} zzHashSet;

/** @brief Initialize HashSet */
zzOpResult zzHashSetInit(zzHashSet *s, size_t keySize, size_t capacity,
             zzHashFn hashFn, zzEqualsFn equalsFn, zzFreeFn keyFree);

/** @brief Free HashSet */
void zzHashSetFree(zzHashSet *s);

/** @brief Insert key into set */
zzOpResult zzHashSetInsert(zzHashSet *s, const void *key);

/** @brief Check if key exists */
bool zzHashSetContains(const zzHashSet *s, const void *key);

/** @brief Remove key from set */
zzOpResult zzHashSetRemove(zzHashSet *s, const void *key);

/** @brief Clear all elements */
void zzHashSetClear(zzHashSet *s);

/** @brief Iterate over all keys */
void zzHashSetForEach(zzHashSet *s, zzForEachFn fn, void *userdata);

/** @brief Union: s1 = s1 ∪ s2 */
zzOpResult zzHashSetUnion(zzHashSet *s1, const zzHashSet *s2);

/** @brief Intersection */
zzOpResult zzHashSetIntersection(const zzHashSet *s1, const zzHashSet *s2, zzHashSet *result,
                     zzHashFn hashFn, zzEqualsFn equalsFn, zzFreeFn keyFree);

/** @brief Difference: result = s1 - s2 */
zzOpResult zzHashSetDifference(const zzHashSet *s1, const zzHashSet *s2, zzHashSet *result,
                   zzHashFn hashFn, zzEqualsFn equalsFn, zzFreeFn keyFree);

#endif
