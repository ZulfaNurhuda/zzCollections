/**
 * @file circularBuffer.h
 * @brief Fixed-size circular buffer implementation for efficient FIFO operations.
 *
 * This module implements a circular buffer (ring buffer) that provides efficient
 * FIFO (First In, First Out) operations with O(1) time complexity for both
 * insertion and removal. The buffer has a fixed capacity and overwrites the oldest
 * elements when full. The implementation supports generic element types and includes
 * memory management through customizable free functions.
 */

#ifndef CIRCULAR_BUFFER_H
#define CIRCULAR_BUFFER_H

#include "types.h"
#include "utils.h"
#include "result.h"
#include "iterator.h"

/**
 * @brief Structure representing a circular buffer.
 *
 * This structure stores elements in a fixed-size buffer that wraps around when
 * reaching its boundaries. It tracks the current size, capacity, and positions
 * of the head (oldest element) and tail (newest element) elements.
 */
typedef struct zzCircularBuffer {
    void *buffer;      /**< Pointer to the underlying buffer storing elements */
    size_t capacity;   /**< Maximum number of elements the buffer can hold */
    size_t head;       /**< Index of the oldest element in the buffer */
    size_t tail;       /**< Index of the next position to insert an element */
    size_t size;       /**< Current number of elements in the buffer */
    size_t elSize;     /**< Size in bytes of each individual element */
    zzFreeFn elemFree; /**< Function to free individual elements, or NULL if not needed */
} zzCircularBuffer;

/**
 * @brief Structure representing an iterator for CircularBuffer.
 *
 * This structure provides iteration through a CircularBuffer from
 * the oldest element to the newest element.
 */
typedef struct zzCircularBufferIterator {
    zzCircularBuffer *buffer;       /**< Pointer to the CircularBuffer being iterated */
    size_t index;                   /**< Current logical index (0 to size-1) */
    zzIteratorState state;          /**< Current state of the iterator */
} zzCircularBufferIterator;

/**
 * @brief Initializes a new CircularBuffer with the specified element size and capacity.
 *
 * This function initializes a CircularBuffer structure with the given element size
 * and capacity. The buffer will be empty after initialization.
 *
 * @param[out] cb Pointer to the CircularBuffer structure to initialize
 * @param[in] elSize Size in bytes of each element that will be stored in the buffer
 * @param[in] capacity Capacity of the buffer (must be greater than 0)
 * @param[in] elemFree Function to free individual elements when they are removed or the buffer is freed, or NULL if not needed
 * @return zzOpResult with status ZZ_SUCCESS on success, or ZZ_ERROR with error message on failure
 */
zzOpResult zzCircularBufferInit(zzCircularBuffer *cb, size_t elSize, size_t capacity, zzFreeFn elemFree);

/**
 * @brief Frees all resources associated with the CircularBuffer.
 *
 * This function releases all memory used by the CircularBuffer, including calling
 * the custom free function for each element if provided. After this function
 * returns, the CircularBuffer structure should not be used until reinitialized.
 *
 * @param[in,out] cb Pointer to the CircularBuffer to free
 */
void zzCircularBufferFree(zzCircularBuffer *cb);

/**
 * @brief Pushes an element to the back of the CircularBuffer.
 *
 * This function adds the specified element to the back of the buffer. If the
 * buffer is full, the oldest element will be overwritten. The element is copied
 * into the buffer's internal storage. If the buffer was full and had a custom
 * free function, it will be called on the overwritten element.
 *
 * @param[in,out] cb Pointer to the CircularBuffer to push to
 * @param[in] elem Pointer to the element to push (contents will be copied)
 * @return zzOpResult with status ZZ_SUCCESS on success, or ZZ_ERROR with error message on failure
 */
zzOpResult zzCircularBufferPush(zzCircularBuffer *cb, const void *elem);

/**
 * @brief Pops an element from the front of the CircularBuffer.
 *
 * This function removes the element from the front of the buffer and copies it
 * to the output buffer. The buffer size is decreased by one. If a custom free
 * function was provided, it will not be called since the element is returned.
 *
 * @param[in,out] cb Pointer to the CircularBuffer to pop from
 * @param[out] out Pointer to a buffer where the popped element will be copied
 * @return zzOpResult with status ZZ_SUCCESS on success, or ZZ_ERROR with error message on failure
 */
zzOpResult zzCircularBufferPop(zzCircularBuffer *cb, void *out);

/**
 * @brief Gets an element at the specified index in the CircularBuffer.
 *
 * This function retrieves the element at the given index in the buffer and copies
 * it to the output buffer. The index is relative to the logical order of elements
 * in the buffer, with index 0 being the front element.
 *
 * @param[in] cb Pointer to the CircularBuffer to retrieve from
 * @param[in] idx Index of the element to retrieve (0-based, relative to front)
 * @param[out] out Pointer to a buffer where the element will be copied
 * @return zzOpResult with status ZZ_SUCCESS on success, or ZZ_ERROR with error message on failure
 */
zzOpResult zzCircularBufferGet(const zzCircularBuffer *cb, size_t idx, void *out);

/**
 * @brief Peeks at the element at the front of the CircularBuffer without removing it.
 *
 * This function copies the element at the front of the buffer to the output buffer
 * without removing it from the buffer. The buffer size remains unchanged.
 *
 * @param[in] cb Pointer to the CircularBuffer to peek into
 * @param[out] out Pointer to a buffer where the front element will be copied
 * @return zzOpResult with status ZZ_SUCCESS on success, or ZZ_ERROR with error message on failure
 */
zzOpResult zzCircularBufferPeekFront(const zzCircularBuffer *cb, void *out);

/**
 * @brief Peeks at the element at the back of the CircularBuffer without removing it.
 *
 * This function copies the element at the back of the buffer to the output buffer
 * without removing it from the buffer. The buffer size remains unchanged.
 *
 * @param[in] cb Pointer to the CircularBuffer to peek into
 * @param[out] out Pointer to a buffer where the back element will be copied
 * @return zzOpResult with status ZZ_SUCCESS on success, or ZZ_ERROR with error message on failure
 */
zzOpResult zzCircularBufferPeekBack(const zzCircularBuffer *cb, void *out);

/**
 * @brief Clears all elements from the CircularBuffer.
 *
 * This function removes all elements from the buffer by calling the custom free
 * function on each element (if provided) and resetting the size to zero.
 * The underlying buffer remains allocated with the same capacity.
 *
 * @param[in,out] cb Pointer to the CircularBuffer to clear
 */
void zzCircularBufferClear(zzCircularBuffer *cb);

/**
 * @brief Initializes an iterator for the CircularBuffer.
 *
 * This function initializes an iterator to traverse the CircularBuffer from
 * the oldest element (head) to the newest element (tail).
 *
 * @param[out] it Pointer to the iterator structure to initialize
 * @param[in] cb Pointer to the CircularBuffer to iterate over
 */
void zzCircularBufferIteratorInit(zzCircularBufferIterator *it, zzCircularBuffer *cb);

/**
 * @brief Advances the iterator to the next element.
 *
 * This function moves the iterator to the next element in the CircularBuffer
 * and copies the current element to the output buffer. Returns false when
 * the iterator reaches the end of the buffer.
 *
 * @param[in,out] it Pointer to the iterator to advance
 * @param[out] valueOut Pointer to a buffer where the current element will be copied
 * @return true if an element was retrieved, false if the iterator reached the end
 */
bool zzCircularBufferIteratorNext(zzCircularBufferIterator *it, void *valueOut);

/**
 * @brief Checks if the iterator has more elements.
 *
 * This function checks whether the iterator can advance to another element
 * without actually advancing it.
 *
 * @param[in] it Pointer to the iterator to check
 * @return true if there are more elements, false otherwise
 */
bool zzCircularBufferIteratorHasNext(const zzCircularBufferIterator *it);

/**
 * @brief Removes the last element returned by the iterator.
 *
 * This function removes the element that was most recently returned by
 * zzCircularBufferIteratorNext. After removal, the iterator remains valid and
 * continues to the next element on the next call to Next.
 *
 * @param[in,out] it Pointer to the iterator
 * @return zzOpResult with status ZZ_SUCCESS on success, or ZZ_ERROR with error message on failure
 */
zzOpResult zzCircularBufferIteratorRemove(zzCircularBufferIterator *it);

#endif