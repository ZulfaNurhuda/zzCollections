/**
 * @file linkedQueue.h
 * @brief Queue implementation using LinkedList as the underlying data structure.
 *
 * This module provides a queue (FIFO - First In, First Out) data structure implemented
 * using LinkedList as the underlying storage. The queue provides O(1) time complexity
 * for enqueue and dequeue operations. The implementation supports generic element types
 * and includes memory management through customizable free functions.
 */

#ifndef LINKED_QUEUE_H
#define LINKED_QUEUE_H

#include "linkedList.h"

/**
 * @brief Type alias for the LinkedQueue, which is implemented using LinkedList.
 */
typedef zzLinkedList zzLinkedQueue;

/**
 * @brief Initializes a new LinkedQueue with the specified element size.
 *
 * This function initializes a LinkedQueue structure with the given element size
 * and custom free function. The queue will be empty after initialization.
 *
 * @param[out] q Pointer to the LinkedQueue structure to initialize
 * @param[in] elSize Size in bytes of each element that will be stored in the queue
 * @param[in] elemFree Function to free individual elements when they are dequeued or the queue is freed, or NULL if not needed
 * @return zzOpResult with status ZZ_SUCCESS on success, or ZZ_ERROR with error message on failure
 */
static inline zzOpResult zzLinkedQueueInit(zzLinkedQueue *q, size_t elSize, zzFreeFn elemFree) {
    return zzLinkedListInit(q, elSize, elemFree);
}

/**
 * @brief Frees all resources associated with the LinkedQueue.
 *
 * This function releases all memory used by the LinkedQueue, including calling
 * the custom free function for each element if provided. After this function
 * returns, the LinkedQueue structure should not be used until reinitialized.
 *
 * @param[in,out] q Pointer to the LinkedQueue to free
 */
static inline void zzLinkedQueueFree(zzLinkedQueue *q) {
    zzLinkedListFree(q);
}

/**
 * @brief Enqueues an element at the back of the LinkedQueue.
 *
 * This function adds the specified element to the back of the queue. The element
 * is copied into a newly allocated node. The queue size is increased by one.
 *
 * @param[in,out] q Pointer to the LinkedQueue to enqueue to
 * @param[in] elem Pointer to the element to enqueue (contents will be copied)
 * @return zzOpResult with status ZZ_SUCCESS on success, or ZZ_ERROR with error message on failure
 */
static inline zzOpResult zzLinkedQueueEnqueue(zzLinkedQueue *q, const void *elem) {
    return zzLinkedListPushBack(q, elem);
}

/**
 * @brief Dequeues an element from the front of the LinkedQueue.
 *
 * This function removes the element from the front of the queue and copies it
 * to the output buffer. The queue size is decreased by one. If a custom free
 * function was provided, it will not be called since the element is returned.
 *
 * @param[in,out] q Pointer to the LinkedQueue to dequeue from
 * @param[out] out Pointer to a buffer where the dequeued element will be copied
 * @return zzOpResult with status ZZ_SUCCESS on success, or ZZ_ERROR with error message on failure
 */
static inline zzOpResult zzLinkedQueueDequeue(zzLinkedQueue *q, void *out) {
    return zzLinkedListPopFront(q, out);
}

/**
 * @brief Peeks at the element at the front of the LinkedQueue without removing it.
 *
 * This function copies the element at the front of the queue to the output buffer
 * without removing it from the queue. The queue size remains unchanged.
 *
 * @param[in] q Pointer to the LinkedQueue to peek into
 * @param[out] out Pointer to a buffer where the front element will be copied
 * @return zzOpResult with status ZZ_SUCCESS on success, or ZZ_ERROR with error message on failure
 */
static inline zzOpResult zzLinkedQueuePeek(const zzLinkedQueue *q, void *out) {
    if (!q) return ZZ_ERR("LinkedList pointer is NULL");
    if (!out) return ZZ_ERR("Output buffer is NULL");
    if (!q->head) return ZZ_ERR("List is empty");

    memcpy(out, q->head->data, q->elSize);
    return ZZ_OK();
}

/**
 * @brief Checks if the LinkedQueue is empty.
 *
 * This function checks whether the queue contains any elements.
 *
 * @param[in] q Pointer to the LinkedQueue to check
 * @return true if the queue is empty (size is 0), false otherwise
 */
static inline bool zzLinkedQueueIsEmpty(const zzLinkedQueue *q) {
    return q->size == 0;
}

/**
 * @brief Gets the current size of the LinkedQueue.
 *
 * This function returns the number of elements currently in the queue.
 *
 * @param[in] q Pointer to the LinkedQueue to get the size of
 * @return The number of elements in the queue
 */
static inline size_t zzLinkedQueueSize(const zzLinkedQueue *q) {
    return q->size;
}

/**
 * @brief Clears all elements from the LinkedQueue.
 *
 * This function removes all elements from the queue by calling the custom free
 * function on each element (if provided) and deallocating all nodes. The queue
 * becomes empty after this operation.
 *
 * @param[in,out] q Pointer to the LinkedQueue to clear
 */
static inline void zzLinkedQueueClear(zzLinkedQueue *q) {
    zzLinkedListClear(q);
}

#endif