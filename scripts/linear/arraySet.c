/**
 * @file arraySet.c
 * @brief Implementation of the dynamic array-based set.
 */

#include "arraySet.h"
#include <string.h>
#include <stdlib.h>

/**
 * @brief Initializes a new ArraySet.
 *
 * Initializes the set with the specified element size and initial capacity.
 *
 * @param[out] as Pointer to the ArraySet to initialize
 * @param[in] elSize Size of each element in bytes
 * @param[in] capacity Initial capacity (will be at least 4)
 * @param[in] equalsFn Function to check element equality (optional, defaults to memcmp)
 * @param[in] elemFree Function to free individual elements (optional)
 * @return zzOpResult ZZ_SUCCESS on success, or error on failure
 */
zzOpResult zzArraySetInit(zzArraySet *as, size_t elSize, size_t capacity, zzEqualsFn equalsFn, zzFreeFn elemFree) {
    if (!as) return ZZ_ERR("ArraySet pointer is NULL");
    if (elSize == 0) return ZZ_ERR("Element size cannot be zero");
    if (capacity == 0) capacity = 4;

    as->elSize = elSize;
    as->size = 0;
    as->capacity = capacity;
    as->equalsFn = equalsFn ? equalsFn : zzDefaultEquals;
    as->elemFree = elemFree;
    as->buffer = malloc(elSize * capacity);

    if (!as->buffer) return ZZ_ERR("Failed to allocate buffer");
    return ZZ_OK();
}

/**
 * @brief Frees the ArraySet and its elements.
 *
 * Releases all memory associated with the set. Calls elemFree on each element if provided.
 *
 * @param[in,out] as Pointer to the ArraySet to free
 */
void zzArraySetFree(zzArraySet *as) {
    if (!as || !as->buffer) return;

    if (as->elemFree) {
        for (size_t i = 0; i < as->size; i++) {
            void *elem = (char*)as->buffer + i * as->elSize;
            as->elemFree(elem);
        }
    }
    free(as->buffer);
    as->buffer = NULL;
    as->size = 0;
}

/**
 * @brief Internal function to grow the ArraySet buffer.
 *
 * @param[in,out] as Pointer to the ArraySet
 * @return zzOpResult ZZ_SUCCESS on success
 */
static zzOpResult zzArraySetGrow(zzArraySet *as) {
    size_t newCap = as->capacity * 2;
    void *newBuf = realloc(as->buffer, as->elSize * newCap);
    if (!newBuf) return ZZ_ERR("Failed to grow buffer");
    as->buffer = newBuf;
    as->capacity = newCap;
    return ZZ_OK();
}

/**
 * @brief Checks if the set contains an element.
 *
 * Performs a linear scan to find the element.
 *
 * @param[in] as Pointer to the ArraySet
 * @param[in] elem Element to check
 * @return true if found, false otherwise
 */
bool zzArraySetContains(const zzArraySet *as, const void *elem) {
    if (!as || !elem) return false;
    for (size_t i = 0; i < as->size; i++) {
        void *current = (char*)as->buffer + i * as->elSize;
        if (as->equalsFn(current, elem)) return true;
    }
    return false;
}

/**
 * @brief Adds an element if it is not already present.
 *
 * Performs a linear scan O(n) to check for duplicates before adding.
 *
 * @param[in,out] as Pointer to the ArraySet
 * @param[in] elem Element to add
 * @return zzOpResult with status ZZ_SUCCESS if added, or error if duplicate or failure
 */
zzOpResult zzArraySetAdd(zzArraySet *as, const void *elem) {
    if (!as) return ZZ_ERR("ArraySet pointer is NULL");
    if (!elem) return ZZ_ERR("Element pointer is NULL");

    if (zzArraySetContains(as, elem)) {
        return ZZ_ERR("Element already exists");
    }

    if (as->size == as->capacity) {
        zzOpResult grow = zzArraySetGrow(as);
        if (ZZ_IS_ERR(grow)) return grow;
    }

    memcpy((char*)as->buffer + as->size * as->elSize, elem, as->elSize);
    as->size++;
    return ZZ_OK();
}

/**
 * @brief Removes an element from the set.
 *
 * Finds and removes the specified element. Shifts remaining elements to fill the gap.
 *
 * @param[in,out] as Pointer to the ArraySet
 * @param[in] elem Element to remove
 * @return zzOpResult with status ZZ_SUCCESS if removed, or error if not found
 */
zzOpResult zzArraySetRemove(zzArraySet *as, const void *elem) {
    if (!as) return ZZ_ERR("ArraySet pointer is NULL");
    
    for (size_t i = 0; i < as->size; i++) {
        void *current = (char*)as->buffer + i * as->elSize;
        if (as->equalsFn(current, elem)) {
            if (as->elemFree) as->elemFree(current);
            
            if (i < as->size - 1) {
                memmove(current, (char*)current + as->elSize, (as->size - i - 1) * as->elSize);
            }
            as->size--;
            return ZZ_OK();
        }
    }
    return ZZ_ERR("Element not found");
}

/**
 * @brief Gets an element at a specific index.
 *
 * Provides read-only access to an element by index. Useful for iteration.
 *
 * @param[in] as Pointer to the ArraySet
 * @param[in] idx Index of the element
 * @param[out] out Output buffer to copy the element into
 * @return zzOpResult with status ZZ_SUCCESS on success, or error if out of bounds
 */
zzOpResult zzArraySetGet(const zzArraySet *as, size_t idx, void *out) {
    if (!as) return ZZ_ERR("ArraySet pointer is NULL");
    if (idx >= as->size) return ZZ_ERR("Index out of bounds");
    memcpy(out, (char*)as->buffer + idx * as->elSize, as->elSize);
    return ZZ_OK();
}

/**
 * @brief Clears the set.
 *
 * Removes all elements and resets size to zero. Capacity remains unchanged.
 *
 * @param[in,out] as Pointer to the ArraySet to clear
 */
void zzArraySetClear(zzArraySet *as) {
    if (!as) return;
    if (as->elemFree) {
        for (size_t i = 0; i < as->size; i++) {
            as->elemFree((char*)as->buffer + i * as->elSize);
        }
    }
    as->size = 0;
}

/**
 * @brief Initializes an iterator for the ArraySet.
 *
 * Prepares an iterator to traverse the set from the beginning.
 *
 * @param[out] it Iterator structure to initialize
 * @param[in] as ArraySet to iterate over
 */
void zzArraySetIteratorInit(zzArraySetIterator *it, zzArraySet *as) {
    if (!it || !as) return;
    it->set = as;
    it->index = 0;
    it->state = (as->size > 0) ? ZZ_ITER_VALID : ZZ_ITER_END;
}

/**
 * @brief Advances the iterator to the next element.
 *
 * Moves to the next element and retrieves it.
 *
 * @param[in,out] it Iterator to advance
 * @param[out] valueOut Buffer to store the retrieved element
 * @return true if an element was retrieved, false if end of set reached
 */
bool zzArraySetIteratorNext(zzArraySetIterator *it, void *valueOut) {
    if (!it || !valueOut || it->state != ZZ_ITER_VALID) return false;
    if (it->index >= it->set->size) {
        it->state = ZZ_ITER_END;
        return false;
    }

    void *elem = (char*)it->set->buffer + it->index * it->set->elSize;
    memcpy(valueOut, elem, it->set->elSize);
    
    it->index++;
    return true;
}

/**
 * @brief Checks if iterator has more elements.
 *
 * Checks if the iterator can advance further.
 *
 * @param[in] it Iterator to check
 * @return true if more elements exist, false otherwise
 */
bool zzArraySetIteratorHasNext(const zzArraySetIterator *it) {
    return it && it->state == ZZ_ITER_VALID && it->index < it->set->size;
}

/**
 * @brief Removes the last element returned by the iterator.
 *
 * Removes the element most recently returned by Next. Safe to call during iteration.
 *
 * @param[in,out] it Iterator to remove from
 * @return zzOpResult with status ZZ_SUCCESS on success, or error if state invalid
 */
zzOpResult zzArraySetIteratorRemove(zzArraySetIterator *it) {
    if (!it || it->state != ZZ_ITER_VALID) return ZZ_ERR("Invalid iterator state");
    if (it->index == 0) return ZZ_ERR("No element to remove");

    size_t removeIdx = it->index - 1;
    zzArraySet *as = it->set;
    
    void *target = (char*)as->buffer + removeIdx * as->elSize;
    if (as->elemFree) as->elemFree(target);

    if (removeIdx < as->size - 1) {
        memmove(target, (char*)target + as->elSize, (as->size - removeIdx - 1) * as->elSize);
    }
    
    as->size--;
    it->index--;
    
    if (it->index >= as->size) {
        if (as->size == 0) it->state = ZZ_ITER_END;
    }

    return ZZ_OK();
}