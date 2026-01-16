/**
 * @file priorityQueue.h
 * @brief Min-heap priority queue implementation
 *
 * PriorityQueue provides O(log n) push/pop operations with O(1) peek.
 * Always returns the minimum element according to comparison function.
 */

#ifndef PRIORITY_QUEUE_H
#define PRIORITY_QUEUE_H

#include "types.h"
#include "utils.h"
#include "result.h"

/**
 * @brief PriorityQueue provides priority-based element ordering using min-heap
 *
 * Binary heap implementation with O(log n) insertion and removal.
 * Elements are ordered by comparison function (smallest/highest priority first).
 */
typedef struct zzPriorityQueue {
    void *buffer;
    size_t elSize;
    size_t size;
    size_t capacity;
    zzCompareFn compareFn;
    zzFreeFn elemFree;
} zzPriorityQueue;

/**
 * @brief Initialize PriorityQueue with specified element size and capacity
 *
 * Allocates heap array. Comparison function is required.
 * Must be freed with zzPriorityQueueFree when done.
 *
 * @param[out] pq Pointer to PriorityQueue to initialize
 * @param[in] elSize Size of each element in bytes (must be > 0)
 * @param[in] capacity Initial capacity (use 0 for default of 16)
 * @param[in] compareFn Comparison function (required, cannot be NULL)
 * @param[in] elemFree Optional element cleanup function (can be NULL)
 *
 * @return zzOpResult with status:
 *         - SUCCESS: PriorityQueue initialized successfully
 *         - ERROR: Possible errors:
 *           * "PriorityQueue pointer is NULL"
 *           * "Element size cannot be zero"
 *           * "Comparison function is NULL"
 *           * "Failed to allocate buffer memory"
 *
 * @see zzPriorityQueueFree
 */
zzOpResult zzPriorityQueueInit(zzPriorityQueue *pq, size_t elSize, size_t capacity,
                       zzCompareFn compareFn, zzFreeFn elemFree);

/**
 * @brief Free all memory associated with PriorityQueue
 *
 * Calls elemFree on all elements (if provided), then frees buffer.
 * The PriorityQueue struct itself is not freed.
 *
 * @param[in,out] pq Pointer to PriorityQueue to free
 *
 * @note Safe to call with NULL pointer (no-op)
 */
void zzPriorityQueueFree(zzPriorityQueue *pq);

/**
 * @brief Push element into priority queue
 *
 * Inserts element and maintains heap property via heapify-up.
 * Automatically grows buffer if at capacity.
 *
 * @param[in,out] pq Pointer to PriorityQueue
 * @param[in] elem Pointer to element data (will be copied)
 *
 * @return zzOpResult with status:
 *         - SUCCESS: Element pushed successfully
 *         - ERROR: Possible errors:
 *           * "PriorityQueue pointer is NULL"
 *           * "Element pointer is NULL"
 *           * "Failed to grow buffer (realloc failed)"
 */
zzOpResult zzPriorityQueuePush(zzPriorityQueue *pq, const void *elem);

/**
 * @brief Pop highest priority element from queue
 *
 * Removes root element and restores heap property via heapify-down.
 *
 * @param[in,out] pq Pointer to PriorityQueue
 * @param[out] out Buffer where element will be copied (must be elSize bytes)
 *
 * @return zzOpResult with status:
 *         - SUCCESS: Element popped successfully
 *         - ERROR: Possible errors:
 *           * "PriorityQueue pointer is NULL"
 *           * "Output buffer is NULL"
 *           * "Queue is empty"
 */
zzOpResult zzPriorityQueuePop(zzPriorityQueue *pq, void *out);

/**
 * @brief Peek at highest priority element without removing
 *
 * Returns root element without modifying queue.
 *
 * @param[in] pq Pointer to PriorityQueue
 * @param[out] out Buffer where element will be copied (must be elSize bytes)
 *
 * @return zzOpResult with status:
 *         - SUCCESS: Element peeked successfully
 *         - ERROR: Possible errors:
 *           * "PriorityQueue pointer is NULL"
 *           * "Output buffer is NULL"
 *           * "Queue is empty"
 */
zzOpResult zzPriorityQueuePeek(const zzPriorityQueue *pq, void *out);

/**
 * @brief Clear all elements from queue
 *
 * Removes all elements (calls elemFree if provided).
 * Keeps allocated buffer. Size becomes 0, capacity unchanged.
 *
 * @param[in,out] pq Pointer to PriorityQueue
 */
void zzPriorityQueueClear(zzPriorityQueue *pq);

#endif
