/**
 * @file priorityQueue.h
 * @brief Priority queue implementation using binary heap for efficient priority-based operations.
 *
 * This module implements a priority queue that maintains elements in a binary heap
 * structure, providing O(log n) time complexity for insertion and removal operations.
 * The highest priority element (based on the comparison function) is always at the top.
 * The implementation supports generic element types and includes memory management
 * through customizable free functions.
 */

#ifndef PRIORITY_QUEUE_H
#define PRIORITY_QUEUE_H

#include "types.h"
#include "utils.h"
#include "result.h"
#include "iterator.h"

/**
 * @brief Structure representing a priority queue.
 *
 * This structure stores elements in a binary heap structure that maintains the
 * heap property. Elements are ordered based on the comparison function provided
 * during initialization, with the highest priority element at the root.
 */
typedef struct zzPriorityQueue {
    void *buffer;       /**< Pointer to the underlying buffer storing elements in heap order */
    size_t size;        /**< Current number of elements in the priority queue */
    size_t capacity;    /**< Maximum number of elements the buffer can hold */
    size_t elSize;      /**< Size in bytes of each individual element */
    zzCompareFn compareFn; /**< Function to compare elements for priority ordering */
    zzFreeFn elemFree;  /**< Function to free individual elements, or NULL if not needed */
} zzPriorityQueue;

/**
 * @brief Structure representing an iterator for PriorityQueue.
 *
 * This structure provides iteration through a PriorityQueue in heap order.
 * Note: The iteration order is not guaranteed to be in priority order.
 */
typedef struct zzPriorityQueueIterator {
    const zzPriorityQueue *queue; /**< Pointer to the PriorityQueue being iterated */
    size_t index;                 /**< Current index in the heap array */
    zzIteratorState state;        /**< Current state of the iterator */
} zzPriorityQueueIterator;

/**
 * @brief Initializes a new PriorityQueue with the specified element size and capacity.
 *
 * This function initializes a PriorityQueue structure with the given element size,
 * initial capacity, and comparison function. The priority queue will be empty after
 * initialization.
 *
 * @param[out] pq Pointer to the PriorityQueue structure to initialize
 * @param[in] elSize Size in bytes of each element that will be stored in the queue
 * @param[in] capacity Initial capacity of the queue (will be adjusted to at least 16)
 * @param[in] compareFn Function to compare elements for priority ordering (returns negative if a<b, 0 if a==b, positive if a>b)
 * @param[in] elemFree Function to free individual elements when they are removed or the queue is freed, or NULL if not needed
 * @return zzOpResult with status ZZ_SUCCESS on success, or ZZ_ERROR with error message on failure
 */
zzOpResult zzPriorityQueueInit(zzPriorityQueue *pq, size_t elSize, size_t capacity, zzCompareFn compareFn, zzFreeFn elemFree);

/**
 * @brief Frees all resources associated with the PriorityQueue.
 *
 * This function releases all memory used by the PriorityQueue, including calling
 * the custom free function for each element if provided. After this function
 * returns, the PriorityQueue structure should not be used until reinitialized.
 *
 * @param[in,out] pq Pointer to the PriorityQueue to free
 */
void zzPriorityQueueFree(zzPriorityQueue *pq);

/**
 * @brief Pushes an element into the PriorityQueue.
 *
 * This function adds the specified element to the priority queue and maintains the
 * heap property by bubbling the element up to its correct position. The element
 * is copied into the queue's internal storage.
 *
 * @param[in,out] pq Pointer to the PriorityQueue to push to
 * @param[in] elem Pointer to the element to push (contents will be copied)
 * @return zzOpResult with status ZZ_SUCCESS on success, or ZZ_ERROR with error message on failure
 */
zzOpResult zzPriorityQueuePush(zzPriorityQueue *pq, const void *elem);

/**
 * @brief Pops the highest priority element from the PriorityQueue.
 *
 * This function removes the highest priority element (the root of the heap) from
 * the priority queue and copies it to the output buffer. The heap property is
 * maintained by moving the last element to the root and sinking it to its correct
 * position. The queue size is decreased by one.
 *
 * @param[in,out] pq Pointer to the PriorityQueue to pop from
 * @param[out] out Pointer to a buffer where the popped element will be copied
 * @return zzOpResult with status ZZ_SUCCESS on success, or ZZ_ERROR with error message on failure
 */
zzOpResult zzPriorityQueuePop(zzPriorityQueue *pq, void *out);

/**
 * @brief Peeks at the highest priority element in the PriorityQueue without removing it.
 *
 * This function copies the highest priority element (the root of the heap) to the
 * output buffer without removing it from the priority queue. The queue size remains
 * unchanged.
 *
 * @param[in] pq Pointer to the PriorityQueue to peek into
 * @param[out] out Pointer to a buffer where the highest priority element will be copied
 * @return zzOpResult with status ZZ_SUCCESS on success, or ZZ_ERROR with error message on failure
 */
zzOpResult zzPriorityQueuePeek(const zzPriorityQueue *pq, void *out);

/**
 * @brief Clears all elements from the PriorityQueue.
 *
 * This function removes all elements from the priority queue by calling the custom free
 * function on each element (if provided) and resetting the size to zero.
 * The underlying buffer remains allocated with the same capacity.
 *
 * @param[in,out] pq Pointer to the PriorityQueue to clear
 */
void zzPriorityQueueClear(zzPriorityQueue *pq);

/**
 * @brief Initializes an iterator for the PriorityQueue.
 *
 * This function initializes an iterator to traverse the PriorityQueue.
 * Note: The iteration order follows the internal heap structure and is
 * not guaranteed to be in priority order.
 *
 * @param[out] it Pointer to the iterator structure to initialize
 * @param[in] pq Pointer to the PriorityQueue to iterate over
 */
void zzPriorityQueueIteratorInit(zzPriorityQueueIterator *it, const zzPriorityQueue *pq);

/**
 * @brief Advances the iterator to the next element.
 *
 * This function moves the iterator to the next element in the PriorityQueue
 * and copies the current element to the output buffer. Returns false when
 * the iterator reaches the end of the queue.
 *
 * @param[in,out] it Pointer to the iterator to advance
 * @param[out] valueOut Pointer to a buffer where the current element will be copied
 * @return true if an element was retrieved, false if the iterator reached the end
 */
bool zzPriorityQueueIteratorNext(zzPriorityQueueIterator *it, void *valueOut);

/**
 * @brief Checks if the iterator has more elements.
 *
 * This function checks whether the iterator can advance to another element
 * without actually advancing it.
 *
 * @param[in] it Pointer to the iterator to check
 * @return true if there are more elements, false otherwise
 */
bool zzPriorityQueueIteratorHasNext(const zzPriorityQueueIterator *it);

#endif