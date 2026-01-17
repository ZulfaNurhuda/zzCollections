/**
 * @file arrayQueue.h
 * @brief Queue implementation using ArrayDeque as the underlying data structure.
 *
 * This module provides a queue (FIFO - First In, First Out) data structure implemented
 * using ArrayDeque as the underlying storage. The queue provides O(1) time complexity
 * for enqueue and dequeue operations. The implementation supports generic element types
 * and includes memory management through customizable free functions.
 */

#ifndef ARRAY_QUEUE_H
#define ARRAY_QUEUE_H

#include "arrayDeque.h"

/**
 * @brief Type alias for the ArrayQueue, which is implemented using ArrayDeque.
 */
typedef zzArrayDeque zzArrayQueue;

/**
 * @brief Initializes a new ArrayQueue with the specified element size and capacity.
 *
 * This function initializes an ArrayQueue structure with the given element size
 * and initial capacity. The capacity will be automatically increased as needed
 * when elements are enqueued.
 *
 * @param[out] q Pointer to the ArrayQueue structure to initialize
 * @param[in] elSize Size in bytes of each element that will be stored in the queue
 * @param[in] capacity Initial capacity of the queue (will be adjusted to at least 4)
 * @param[in] elemFree Function to free individual elements when they are dequeued or the queue is freed, or NULL if not needed
 * @return zzOpResult with status ZZ_SUCCESS on success, or ZZ_ERROR with error message on failure
 */
static inline zzOpResult zzArrayQueueInit(zzArrayQueue *q, size_t elSize, size_t capacity, zzFreeFn elemFree) {
    return zzArrayDequeInit(q, elSize, capacity, elemFree);
}

/**
 * @brief Frees all resources associated with the ArrayQueue.
 *
 * This function releases all memory used by the ArrayQueue, including calling
 * the custom free function for each element if provided. After this function
 * returns, the ArrayQueue structure should not be used until reinitialized.
 *
 * @param[in,out] q Pointer to the ArrayQueue to free
 */
static inline void zzArrayQueueFree(zzArrayQueue *q) {
    zzArrayDequeFree(q);
}

/**
 * @brief Enqueues an element at the back of the ArrayQueue.
 *
 * This function adds the specified element to the back of the queue. If the
 * queue has reached its capacity, it will be automatically resized to accommodate
 * the new element. The element is copied into the queue's internal buffer.
 *
 * @param[in,out] q Pointer to the ArrayQueue to enqueue to
 * @param[in] elem Pointer to the element to enqueue (contents will be copied)
 * @return zzOpResult with status ZZ_SUCCESS on success, or ZZ_ERROR with error message on failure
 */
static inline zzOpResult zzArrayQueueEnqueue(zzArrayQueue *q, const void *elem) {
    return zzArrayDequePushBack(q, elem);
}

/**
 * @brief Dequeues an element from the front of the ArrayQueue.
 *
 * This function removes the element from the front of the queue and copies it
 * to the output buffer. The queue size is decreased by one. If a custom free
 * function was provided, it will not be called since the element is returned.
 *
 * @param[in,out] q Pointer to the ArrayQueue to dequeue from
 * @param[out] out Pointer to a buffer where the dequeued element will be copied
 * @return zzOpResult with status ZZ_SUCCESS on success, or ZZ_ERROR with error message on failure
 */
static inline zzOpResult zzArrayQueueDequeue(zzArrayQueue *q, void *out) {
    return zzArrayDequePopFront(q, out);
}

/**
 * @brief Peeks at the element at the front of the ArrayQueue without removing it.
 *
 * This function copies the element at the front of the queue to the output buffer
 * without removing it from the queue. The queue size remains unchanged.
 *
 * @param[in] q Pointer to the ArrayQueue to peek into
 * @param[out] out Pointer to a buffer where the front element will be copied
 * @return zzOpResult with status ZZ_SUCCESS on success, or ZZ_ERROR with error message on failure
 */
static inline zzOpResult zzArrayQueuePeek(const zzArrayQueue *q, void *out) {
    return zzArrayDequePeekFront(q, out);
}

/**
 * @brief Checks if the ArrayQueue is empty.
 *
 * This function checks whether the queue contains any elements.
 *
 * @param[in] q Pointer to the ArrayQueue to check
 * @return true if the queue is empty (size is 0), false otherwise
 */
static inline bool zzArrayQueueIsEmpty(const zzArrayQueue *q) {
    return q->size == 0;
}

/**
 * @brief Gets the current size of the ArrayQueue.
 *
 * This function returns the number of elements currently in the queue.
 *
 * @param[in] q Pointer to the ArrayQueue to get the size of
 * @return The number of elements in the queue
 */
static inline size_t zzArrayQueueSize(const zzArrayQueue *q) {
    return q->size;
}

/**
 * @brief Clears all elements from the ArrayQueue.
 *
 * This function removes all elements from the queue by calling the custom free
 * function on each element (if provided) and resetting the size to zero.
 * The underlying buffer remains allocated with the same capacity.
 *
 * @param[in,out] q Pointer to the ArrayQueue to clear
 */
static inline void zzArrayQueueClear(zzArrayQueue *q) {
    zzArrayDequeClear(q);
}

#endif