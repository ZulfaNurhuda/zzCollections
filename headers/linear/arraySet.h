/**
 * @file arraySet.h
 * @brief Dynamic array-based set implementation (Flat Set).
 *
 * This module implements a set data structure using a dynamic array.
 * It ensures uniqueness by performing a linear scan O(n) before adding elements.
 * This structure is efficient for small datasets where the overhead of hashing
 * or tree structures is unnecessary.
 */

#ifndef ARRAY_SET_H
#define ARRAY_SET_H

#include "types.h"
#include "utils.h"
#include "result.h"
#include "iterator.h"

/**
 * @brief Structure representing a dynamic array-based set.
 *
 * Stores unique elements in a contiguous memory block.
 * Growth is handled automatically. Uniqueness is enforced via linear search.
 */
typedef struct zzArraySet {
    void *buffer;          /**< Pointer to the underlying buffer */
    size_t size;           /**< Current number of elements */
    size_t capacity;       /**< Maximum capacity before resize */
    size_t elSize;         /**< Size of each element in bytes */
    zzEqualsFn equalsFn;   /**< Function to check element equality */
    zzFreeFn elemFree;     /**< Function to free elements */
} zzArraySet;

/**
 * @brief Structure representing an iterator for ArraySet.
 *
 * This structure provides iteration capability for the ArraySet.
 * It maintains the current index and state of iteration.
 */
typedef struct zzArraySetIterator {
    zzArraySet *set;       /**< Pointer to the ArraySet */
    size_t index;          /**< Current index */
    zzIteratorState state; /**< Iterator state */
} zzArraySetIterator;

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
zzOpResult zzArraySetInit(zzArraySet *as, size_t elSize, size_t capacity, zzEqualsFn equalsFn, zzFreeFn elemFree);

/**
 * @brief Frees the ArraySet and its elements.
 *
 * Releases all memory associated with the set. Calls elemFree on each element if provided.
 *
 * @param[in,out] as Pointer to the ArraySet to free
 */
void zzArraySetFree(zzArraySet *as);

/**
 * @brief Adds an element if it is not already present.
 *
 * Performs a linear scan O(n) to check for duplicates before adding.
 *
 * @param[in,out] as Pointer to the ArraySet
 * @param[in] elem Element to add
 * @return zzOpResult with status ZZ_SUCCESS if added, or error if duplicate or failure
 */
zzOpResult zzArraySetAdd(zzArraySet *as, const void *elem);

/**
 * @brief Removes an element from the set.
 *
 * Finds and removes the specified element. Shifts remaining elements to fill the gap.
 *
 * @param[in,out] as Pointer to the ArraySet
 * @param[in] elem Element to remove
 * @return zzOpResult with status ZZ_SUCCESS if removed, or error if not found
 */
zzOpResult zzArraySetRemove(zzArraySet *as, const void *elem);

/**
 * @brief Checks if the set contains an element.
 *
 * Performs a linear scan to find the element.
 *
 * @param[in] as Pointer to the ArraySet
 * @param[in] elem Element to check
 * @return true if found, false otherwise
 */
bool zzArraySetContains(const zzArraySet *as, const void *elem);

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
zzOpResult zzArraySetGet(const zzArraySet *as, size_t idx, void *out);

/**
 * @brief Clears the set.
 *
 * Removes all elements and resets size to zero. Capacity remains unchanged.
 *
 * @param[in,out] as Pointer to the ArraySet to clear
 */
void zzArraySetClear(zzArraySet *as);

/**
 * @brief Initializes an iterator for the ArraySet.
 *
 * Prepares an iterator to traverse the set from the beginning.
 *
 * @param[out] it Iterator structure to initialize
 * @param[in] as ArraySet to iterate over
 */
void zzArraySetIteratorInit(zzArraySetIterator *it, zzArraySet *as);

/**
 * @brief Advances the iterator to the next element.
 *
 * Moves to the next element and retrieves it.
 *
 * @param[in,out] it Iterator to advance
 * @param[out] valueOut Buffer to store the retrieved element
 * @return true if an element was retrieved, false if end of set reached
 */
bool zzArraySetIteratorNext(zzArraySetIterator *it, void *valueOut);

/**
 * @brief Checks if iterator has more elements.
 *
 * Checks if the iterator can advance further.
 *
 * @param[in] it Iterator to check
 * @return true if more elements exist, false otherwise
 */
bool zzArraySetIteratorHasNext(const zzArraySetIterator *it);

/**
 * @brief Removes the last element returned by the iterator.
 *
 * Removes the element most recently returned by Next. Safe to call during iteration.
 *
 * @param[in,out] it Iterator to remove from
 * @return zzOpResult with status ZZ_SUCCESS on success, or error if state invalid
 */
zzOpResult zzArraySetIteratorRemove(zzArraySetIterator *it);

#endif