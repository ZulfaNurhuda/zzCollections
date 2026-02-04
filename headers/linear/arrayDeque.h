/**
 * @file arrayDeque.h
 * @brief Circular buffer-based double-ended queue (deque) implementation.
 *
 * This module implements a double-ended queue (deque) using a circular buffer
 * approach. Elements can be efficiently added or removed from both ends of the
 * deque in O(1) amortized time. The underlying buffer grows dynamically as needed.
 */

#ifndef ARRAY_DEQUE_H
#define ARRAY_DEQUE_H

#include "types.h"
#include "utils.h"
#include "result.h"
#include "iterator.h"

/**
 * @brief Structure representing a double-ended queue (ArrayDeque).
 *
 * This structure implements a deque using a circular buffer approach, where
 * elements can be added or removed from both ends efficiently. The buffer
 * wraps around when reaching its boundaries, maximizing space utilization.
 */
typedef struct zzArrayDeque {
    void *buffer;      /**< Pointer to the underlying circular buffer storing elements */
    size_t capacity;   /**< Maximum number of elements the buffer can hold */
    size_t front;      /**< Index of the front element in the circular buffer */
    size_t size;       /**< Current number of elements in the deque */
    size_t elSize;     /**< Size in bytes of each individual element */
    zzFreeFn elemFree; /**< Function to free individual elements, or NULL if not needed */
} zzArrayDeque;

/**
 * @brief Structure representing an iterator for ArrayDeque.
 *
 * This structure provides forward iteration through an ArrayDeque,
 * maintaining the current position and reference to the deque.
 */
typedef struct zzArrayDequeIterator {
    zzArrayDeque *deque;       /**< Pointer to the ArrayDeque being iterated */
    size_t index;              /**< Current logical index in the deque */
    zzIteratorState state;     /**< Current state of the iterator */
} zzArrayDequeIterator;

/**
 * @brief Initializes a new ArrayDeque with the specified element size and capacity.
 *
 * This function initializes an ArrayDeque structure with the given element size
 * and initial capacity. The capacity will be automatically increased as needed
 * when elements are added to the deque.
 *
 * @param[out] ad Pointer to the ArrayDeque structure to initialize
 * @param[in] elSize Size in bytes of each element that will be stored in the deque
 * @param[in] capacity Initial capacity of the deque (will be adjusted to at least 4)
 * @param[in] elemFree Function to free individual elements when they are removed or the deque is freed, or NULL if not needed
 * @return zzOpResult with status ZZ_SUCCESS on success, or ZZ_ERROR with error message on failure
 */
zzOpResult zzArrayDequeInit(zzArrayDeque *ad, size_t elSize, size_t capacity, zzFreeFn elemFree);

/**
 * @brief Frees all resources associated with the ArrayDeque.
 *
 * This function releases all memory used by the ArrayDeque, including calling
 * the custom free function for each element if provided. After this function
 * returns, the ArrayDeque structure should not be used until reinitialized.
 *
 * @param[in,out] ad Pointer to the ArrayDeque to free
 */
void zzArrayDequeFree(zzArrayDeque *ad);

/**
 * @brief Adds an element to the front of the ArrayDeque.
 *
 * This function adds the specified element to the front of the deque. If the
 * deque has reached its capacity, it will be automatically resized to accommodate
 * the new element. The element is copied into the deque's internal buffer.
 *
 * @param[in,out] ad Pointer to the ArrayDeque to add to
 * @param[in] elem Pointer to the element to add to the front (contents will be copied)
 * @return zzOpResult with status ZZ_SUCCESS on success, or ZZ_ERROR with error message on failure
 */
zzOpResult zzArrayDequePushFront(zzArrayDeque *ad, const void *elem);

/**
 * @brief Adds an element to the back of the ArrayDeque.
 *
 * This function adds the specified element to the back of the deque. If the
 * deque has reached its capacity, it will be automatically resized to accommodate
 * the new element. The element is copied into the deque's internal buffer.
 *
 * @param[in,out] ad Pointer to the ArrayDeque to add to
 * @param[in] elem Pointer to the element to add to the back (contents will be copied)
 * @return zzOpResult with status ZZ_SUCCESS on success, or ZZ_ERROR with error message on failure
 */
zzOpResult zzArrayDequePushBack(zzArrayDeque *ad, const void *elem);

/**
 * @brief Removes an element from the front of the ArrayDeque.
 *
 * This function removes the element from the front of the deque and copies it
 * to the output buffer. The deque size is decreased by one. If a custom free
 * function was provided, it will not be called since the element is returned.
 *
 * @param[in,out] ad Pointer to the ArrayDeque to remove from
 * @param[out] out Pointer to a buffer where the removed element will be copied
 * @return zzOpResult with status ZZ_SUCCESS on success, or ZZ_ERROR with error message on failure
 */
zzOpResult zzArrayDequePopFront(zzArrayDeque *ad, void *out);

/**
 * @brief Removes an element from the back of the ArrayDeque.
 *
 * This function removes the element from the back of the deque and copies it
 * to the output buffer. The deque size is decreased by one. If a custom free
 * function was provided, it will not be called since the element is returned.
 *
 * @param[in,out] ad Pointer to the ArrayDeque to remove from
 * @param[out] out Pointer to a buffer where the removed element will be copied
 * @return zzOpResult with status ZZ_SUCCESS on success, or ZZ_ERROR with error message on failure
 */
zzOpResult zzArrayDequePopBack(zzArrayDeque *ad, void *out);

/**
 * @brief Peeks at the element at the front of the ArrayDeque without removing it.
 *
 * This function copies the element at the front of the deque to the output buffer
 * without removing it from the deque. The deque size remains unchanged.
 *
 * @param[in] ad Pointer to the ArrayDeque to peek into
 * @param[out] out Pointer to a buffer where the front element will be copied
 * @return zzOpResult with status ZZ_SUCCESS on success, or ZZ_ERROR with error message on failure
 */
zzOpResult zzArrayDequePeekFront(const zzArrayDeque *ad, void *out);

/**
 * @brief Peeks at the element at the back of the ArrayDeque without removing it.
 *
 * This function copies the element at the back of the deque to the output buffer
 * without removing it from the deque. The deque size remains unchanged.
 *
 * @param[in] ad Pointer to the ArrayDeque to peek into
 * @param[out] out Pointer to a buffer where the back element will be copied
 * @return zzOpResult with status ZZ_SUCCESS on success, or ZZ_ERROR with error message on failure
 */
zzOpResult zzArrayDequePeekBack(const zzArrayDeque *ad, void *out);

/**
 * @brief Gets an element at the specified index in the ArrayDeque.
 *
 * This function retrieves the element at the given index in the deque and copies
 * it to the output buffer. The index is relative to the logical order of elements
 * in the deque, not their physical positions in the circular buffer.
 *
 * @param[in] ad Pointer to the ArrayDeque to retrieve from
 * @param[in] idx Index of the element to retrieve (0-based, relative to front)
 * @param[out] out Pointer to a buffer where the element will be copied
 * @return zzOpResult with status ZZ_SUCCESS on success, or ZZ_ERROR with error message on failure
 */
zzOpResult zzArrayDequeGet(const zzArrayDeque *ad, size_t idx, void *out);

/**
 * @brief Clears all elements from the ArrayDeque.
 *
 * This function removes all elements from the deque by calling the custom free
 * function on each element (if provided) and resetting the size to zero.
 * The underlying buffer remains allocated with the same capacity.
 *
 * @param[in,out] ad Pointer to the ArrayDeque to clear
 */
void zzArrayDequeClear(zzArrayDeque *ad);

/**
 * @brief Initializes an iterator for the ArrayDeque.
 *
 * This function initializes an iterator to traverse the ArrayDeque from
 * the front to the back. The iterator will be positioned at the first
 * element if the deque is not empty.
 *
 * @param[out] it Pointer to the iterator structure to initialize
 * @param[in] ad Pointer to the ArrayDeque to iterate over
 */
void zzArrayDequeIteratorInit(zzArrayDequeIterator *it, zzArrayDeque *ad);

/**
 * @brief Advances the iterator to the next element.
 *
 * This function moves the iterator to the next element in the ArrayDeque
 * and copies the current element to the output buffer. Returns false when
 * the iterator reaches the end of the deque.
 *
 * @param[in,out] it Pointer to the iterator to advance
 * @param[out] valueOut Pointer to a buffer where the current element will be copied
 * @return true if an element was retrieved, false if the iterator reached the end
 */
bool zzArrayDequeIteratorNext(zzArrayDequeIterator *it, void *valueOut);

/**
 * @brief Checks if the iterator has more elements.
 *
 * This function checks whether the iterator can advance to another element
 * without actually advancing it.
 *
 * @param[in] it Pointer to the iterator to check
 * @return true if there are more elements, false otherwise
 */
bool zzArrayDequeIteratorHasNext(const zzArrayDequeIterator *it);

/**
 * @brief Removes the last element returned by the iterator.
 *
 * This function removes the element that was most recently returned by
 * zzArrayDequeIteratorNext. After removal, the iterator remains valid and
 * continues to the next element on the next call to Next.
 *
 * @param[in,out] it Pointer to the iterator
 * @return zzOpResult with status ZZ_SUCCESS on success, or ZZ_ERROR with error message on failure
 */
zzOpResult zzArrayDequeIteratorRemove(zzArrayDequeIterator *it);

#endif