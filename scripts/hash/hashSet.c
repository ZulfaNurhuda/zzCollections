/**
 * @file hashSet.c
 * @brief Implementation of the hash set data structure using chaining for collision resolution.
 *
 * This module provides the implementation for the HashSet data structure,
 * which offers efficient storage of unique keys with average O(1) time complexity
 * for insertion, lookup, and deletion operations. The implementation handles
 * memory management, collision resolution through chaining, and automatic
 * resizing when the load factor threshold is exceeded.
 */

#include "hashSet.h"
#include <string.h>
#include <stdlib.h>

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
zzOpResult zzHashSetInit(zzHashSet *s, size_t keySize, size_t capacity, zzHashFn hashFn, zzEqualsFn equalsFn, zzFreeFn keyFree) {
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
 * @brief Frees all resources associated with the HashSet.
 *
 * This function releases all memory used by the HashSet, including calling
 * the custom free function for each key if provided. After this function
 * returns, the HashSet structure should not be used until reinitialized.
 *
 * @param[in,out] s Pointer to the HashSet to free
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
 * @brief Internal function to rehash the HashSet when load factor threshold is exceeded.
 *
 * This helper function doubles the capacity of the hash set and redistributes
 * all existing entries to their new positions based on the new hash table size.
 * This maintains the efficiency of the hash set by keeping the load factor low.
 *
 * @param[in,out] s Pointer to the HashSet to rehash
 * @return zzOpResult with status ZZ_SUCCESS on success, or ZZ_ERROR with error message on failure
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
 * @brief Checks if the HashSet contains the specified key.
 *
 * This function checks whether the given key exists in the hash set.
 *
 * @param[in] s Pointer to the HashSet to check
 * @param[in] key Pointer to the key to look for
 * @return true if the key exists in the set, false otherwise
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
 * @brief Clears all keys from the HashSet.
 *
 * This function removes all keys from the hash set by calling the custom free
 * function for each key (if provided) and deallocating all nodes. The hash set
 * becomes empty after this operation.
 *
 * @param[in,out] s Pointer to the HashSet to clear
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
 * @brief Applies a function to each element in the HashSet.
 *
 * This function iterates through all elements in the hash set and applies
 * the provided function to each element, passing along user-defined data.
 *
 * @param[in,out] s Pointer to the HashSet to iterate over
 * @param[in] fn Function to apply to each element
 * @param[in] userdata Pointer to user-defined data to pass to the function
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
 * @brief Performs a union operation between two hash sets.
 *
 * This function adds all elements from the second hash set to the first hash set.
 * After the operation, the first set will contain all elements that were in either set.
 *
 * @param[in,out] s1 Pointer to the first HashSet (destination for the union)
 * @param[in] s2 Pointer to the second HashSet (source for the union)
 * @return zzOpResult with status ZZ_SUCCESS on success, or ZZ_ERROR with error message on failure
 */
zzOpResult zzHashSetUnion(zzHashSet *s1, const zzHashSet *s2) {
    if (!s1) return ZZ_ERR("HashSet s1 pointer is NULL");
    if (!s2) return ZZ_ERR("HashSet s2 pointer is NULL");

    for (size_t i = 0; i < s2->capacity; i++) {
        SetNode *cur = s2->buckets[i];
        while (cur) {
            zzOpResult insertResult = zzHashSetInsert(s1, cur->key);
            if (ZZ_IS_ERR(insertResult) && insertResult.error != NULL) {
                if (insertResult.error[0] != 'K') {
                    return insertResult;
                }
            }
            cur = cur->next;
        }
    }
    return ZZ_OK();
}

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
zzOpResult zzHashSetIntersection(const zzHashSet *s1, const zzHashSet *s2, zzHashSet *result, zzHashFn hashFn, zzEqualsFn equalsFn, zzFreeFn keyFree) {
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
zzOpResult zzHashSetDifference(const zzHashSet *s1, const zzHashSet *s2, zzHashSet *result, zzHashFn hashFn, zzEqualsFn equalsFn, zzFreeFn keyFree) {
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
void zzHashSetIteratorInit(zzHashSetIterator *it, zzHashSet *s) {
    if (!it || !s) return;
    
    it->set = s;
    it->bucketIndex = 0;
    it->currentNode = NULL;
    it->lastReturned = NULL;
    it->state = ZZ_ITER_END;
    
    for (size_t i = 0; i < s->capacity; i++) {
        if (s->buckets[i] != NULL) {
            it->bucketIndex = i;
            it->currentNode = s->buckets[i];
            it->state = ZZ_ITER_VALID;
            break;
        }
    }
}

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
bool zzHashSetIteratorNext(zzHashSetIterator *it, void *keyOut) {
    if (!it || !keyOut || it->state != ZZ_ITER_VALID || !it->currentNode) return false;
    
    memcpy(keyOut, it->currentNode->key, it->set->keySize);
    
    it->lastReturned = it->currentNode;

    it->currentNode = it->currentNode->next;
    
    if (!it->currentNode) {
        it->bucketIndex++;
        while (it->bucketIndex < it->set->capacity && !it->set->buckets[it->bucketIndex]) {
            it->bucketIndex++;
        }
        
        if (it->bucketIndex < it->set->capacity) {
            it->currentNode = it->set->buckets[it->bucketIndex];
        }
    }
    
    return true;
}

/**
 * @brief Checks if the iterator has more elements.
 *
 * This function checks whether the iterator can advance to another key
 * without actually advancing it.
 *
 * @param[in] it Pointer to the iterator to check
 * @return true if there are more elements, false otherwise
 */
bool zzHashSetIteratorHasNext(const zzHashSetIterator *it) {
    return it && it->state == ZZ_ITER_VALID && it->currentNode != NULL;
}

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
zzOpResult zzHashSetIteratorRemove(zzHashSetIterator *it) {
    if (!it || it->state != ZZ_ITER_VALID) return ZZ_ERR("Invalid iterator state");
    if (!it->lastReturned) return ZZ_ERR("No element to remove (Next not called or already removed)");

    SetNode *target = it->lastReturned;
    size_t bucketIdx = target->hash % it->set->capacity;
    
    SetNode **cur = &it->set->buckets[bucketIdx];
    while (*cur) {
        if (*cur == target) {
            *cur = target->next; 
            
            if (it->set->keyFree) it->set->keyFree(target->key);
            
            free(target);
            it->set->size--;
            it->lastReturned = NULL; 
            return ZZ_OK();
        }
        cur = &(*cur)->next;
    }
    
    return ZZ_ERR("Element not found (should not happen)");
}