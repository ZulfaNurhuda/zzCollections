#ifndef ARRAY_QUEUE_H
#define ARRAY_QUEUE_H

#include "arrayDeque.h"

typedef zzArrayDeque zzArrayQueue;

/**
 * @brief Initialize ArrayQueue
 * @see zzArrayDequeInit for detailed documentation
 */
static inline zzOpResult zzArrayQueueInit(zzArrayQueue *q, size_t elSize, size_t capacity, zzFreeFn elemFree) {
    return zzArrayDequeInit(q, elSize, capacity, elemFree);
}

/**
 * @brief Free ArrayQueue
 * @see zzArrayDequeFree for detailed documentation
 */
static inline void zzArrayQueueFree(zzArrayQueue *q) {
    zzArrayDequeFree(q);
}

/**
 * @brief Enqueue element (add to back, FIFO)
 * @see zzArrayDequePushBack for detailed documentation
 */
static inline zzOpResult zzArrayQueueEnqueue(zzArrayQueue *q, const void *elem) {
    return zzArrayDequePushBack(q, elem);
}

/**
 * @brief Dequeue element (remove from front)
 * @see zzArrayDequePopFront for detailed documentation
 */
static inline zzOpResult zzArrayQueueDequeue(zzArrayQueue *q, void *out) {
    return zzArrayDequePopFront(q, out);
}

/**
 * @brief Peek at front element without removing
 * @see zzArrayDequePeekFront for detailed documentation
 */
static inline zzOpResult zzArrayQueuePeek(const zzArrayQueue *q, void *out) {
    return zzArrayDequePeekFront(q, out);
}

/**
 * @brief Check if queue is empty
 * @return true if empty, false otherwise
 */
static inline bool zzArrayQueueIsEmpty(const zzArrayQueue *q) {
    return q->size == 0;
}

/**
 * @brief Get queue size
 * @return Number of elements in queue
 */
static inline size_t zzArrayQueueSize(const zzArrayQueue *q) {
    return q->size;
}

/**
 * @brief Clear all elements from queue
 * @see zzArrayDequeClear for detailed documentation
 */
static inline void zzArrayQueueClear(zzArrayQueue *q) {
    zzArrayDequeClear(q);
}

#endif
