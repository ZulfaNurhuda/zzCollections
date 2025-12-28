#ifndef LINKED_QUEUE_H
#define LINKED_QUEUE_H

#include "linkedList.h"
#include "memory.h"

typedef zzLinkedList zzLinkedQueue;

/**
 * @brief Initialize LinkedQueue
 * @see zzLinkedListInit for detailed documentation
 */
static inline zzOpResult zzLinkedQueueInit(zzLinkedQueue *q, size_t elSize, zzFreeFn elemFree) {
    return zzLinkedListInit(q, elSize, elemFree);
}

/**
 * @brief Free LinkedQueue
 * @see zzLinkedListFree for detailed documentation
 */
static inline void zzLinkedQueueFree(zzLinkedQueue *q) {
    zzLinkedListFree(q);
}

/**
 * @brief Enqueue element (add to back, FIFO)
 * @see zzLinkedListPushBack for detailed documentation
 */
static inline zzOpResult zzLinkedQueueEnqueue(zzLinkedQueue *q, const void *elem) {
    return zzLinkedListPushBack(q, elem);
}

/**
 * @brief Dequeue element (remove from front)
 * @see zzLinkedListPopFront for detailed documentation
 */
static inline zzOpResult zzLinkedQueueDequeue(zzLinkedQueue *q, void *out) {
    return zzLinkedListPopFront(q, out);
}

/**
 * @brief Peek at front element without removing
 *
 * @param[in] q Pointer to LinkedQueue
 * @param[out] out Buffer where element will be copied
 *
 * @return zzOpResult with status:
 *         - SUCCESS: Element peeked successfully
 *         - ERROR: Possible errors:
 *           * "LinkedList pointer is NULL"
 *           * "Output buffer is NULL"
 *           * "List is empty"
 */
static inline zzOpResult zzLinkedQueuePeek(const zzLinkedQueue *q, void *out) {
    if (!q) return ZZ_ERR("LinkedList pointer is NULL");
    if (!out) return ZZ_ERR("Output buffer is NULL");
    if (!q->head) return ZZ_ERR("List is empty");

    zzMemoryCopy(out, q->head->data, q->elSize);
    return ZZ_OK();
}

/**
 * @brief Check if queue is empty
 * @return true if empty, false otherwise
 */
static inline bool zzLinkedQueueIsEmpty(const zzLinkedQueue *q) {
    return q->size == 0;
}

/**
 * @brief Get queue size
 * @return Number of elements in queue
 */
static inline size_t zzLinkedQueueSize(const zzLinkedQueue *q) {
    return q->size;
}

/**
 * @brief Clear all elements from queue
 * @see zzLinkedListClear for detailed documentation
 */
static inline void zzLinkedQueueClear(zzLinkedQueue *q) {
    zzLinkedListClear(q);
}

#endif
