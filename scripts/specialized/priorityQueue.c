#include "priorityQueue.h"
#include <string.h>
#include <stdlib.h>

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
zzOpResult zzPriorityQueueInit(zzPriorityQueue *pq, size_t elSize, size_t capacity, zzCompareFn compareFn, zzFreeFn elemFree) {
    if (!pq) return ZZ_ERR("PriorityQueue pointer is NULL");
    if (elSize == 0) return ZZ_ERR("Element size cannot be zero");
    if (!compareFn) return ZZ_ERR("Comparison function is NULL");
    if (capacity == 0) capacity = 16;

    pq->buffer = malloc(elSize * capacity);
    if (!pq->buffer) return ZZ_ERR("Failed to allocate buffer memory");

    pq->elSize = elSize;
    pq->size = 0;
    pq->capacity = capacity;
    pq->compareFn = compareFn;
    pq->elemFree = elemFree;
    return ZZ_OK();
}

/**
 * @brief Frees all resources associated with the PriorityQueue.
 *
 * This function releases all memory used by the PriorityQueue, including calling
 * the custom free function for each element if provided. After this function
 * returns, the PriorityQueue structure should not be used until reinitialized.
 *
 * @param[in,out] pq Pointer to the PriorityQueue to free
 */
void zzPriorityQueueFree(zzPriorityQueue *pq) {
    if (!pq || !pq->buffer) return;
    
    if (pq->elemFree) {
        for (size_t i = 0; i < pq->size; i++) {
            void *elem = (char*)pq->buffer + i * pq->elSize;
            pq->elemFree(elem);
        }
    }
    free(pq->buffer);
    pq->buffer = NULL;
    pq->size = 0;
}

static zzOpResult zzPriorityQueueResize(zzPriorityQueue *pq) {
    size_t newCap = pq->capacity * 2;
    void *newBuf = realloc(pq->buffer, pq->elSize * newCap);
    if (!newBuf) return ZZ_ERR("Failed to grow buffer (realloc failed)");
    pq->buffer = newBuf;
    pq->capacity = newCap;
    return ZZ_OK();
}

static void heapifyUp(zzPriorityQueue *pq, size_t idx) {
    while (idx > 0) {
        size_t parent = (idx - 1) / 2;
        void *child = (char*)pq->buffer + idx * pq->elSize;
        void *parentElem = (char*)pq->buffer + parent * pq->elSize;
        
        if (pq->compareFn(child, parentElem) >= 0) break;
        
        char tmp[pq->elSize];
        memcpy(tmp, child, pq->elSize);
        memcpy(child, parentElem, pq->elSize);
        memcpy(parentElem, tmp, pq->elSize);
        
        idx = parent;
    }
}

static void heapifyDown(zzPriorityQueue *pq, size_t idx) {
    while (1) {
        size_t smallest = idx;
        size_t left = 2 * idx + 1;
        size_t right = 2 * idx + 2;
        
        if (left < pq->size) {
            void *leftElem = (char*)pq->buffer + left * pq->elSize;
            void *smallestElem = (char*)pq->buffer + smallest * pq->elSize;
            if (pq->compareFn(leftElem, smallestElem) < 0) {
                smallest = left;
            }
        }
        
        if (right < pq->size) {
            void *rightElem = (char*)pq->buffer + right * pq->elSize;
            void *smallestElem = (char*)pq->buffer + smallest * pq->elSize;
            if (pq->compareFn(rightElem, smallestElem) < 0) {
                smallest = right;
            }
        }
        
        if (smallest == idx) break;
        
        void *idxElem = (char*)pq->buffer + idx * pq->elSize;
        void *smallestElem = (char*)pq->buffer + smallest * pq->elSize;
        
        char tmp[pq->elSize];
        memcpy(tmp, idxElem, pq->elSize);
        memcpy(idxElem, smallestElem, pq->elSize);
        memcpy(smallestElem, tmp, pq->elSize);
        
        idx = smallest;
    }
}

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
zzOpResult zzPriorityQueuePush(zzPriorityQueue *pq, const void *elem) {
    if (!pq) return ZZ_ERR("PriorityQueue pointer is NULL");
    if (!elem) return ZZ_ERR("Element pointer is NULL");

    if (pq->size == pq->capacity) {
        zzOpResult resizeResult = zzPriorityQueueResize(pq);
        if (ZZ_IS_ERR(resizeResult)) return resizeResult;
    }

    void *target = (char*)pq->buffer + pq->size * pq->elSize;
    memcpy(target, elem, pq->elSize);
    heapifyUp(pq, pq->size);
    pq->size++;
    return ZZ_OK();
}

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
zzOpResult zzPriorityQueuePop(zzPriorityQueue *pq, void *out) {
    if (!pq) return ZZ_ERR("PriorityQueue pointer is NULL");
    if (!out) return ZZ_ERR("Output buffer is NULL");
    if (pq->size == 0) return ZZ_ERR("Queue is empty");

    memcpy(out, pq->buffer, pq->elSize);

    pq->size--;
    if (pq->size > 0) {
        void *last = (char*)pq->buffer + pq->size * pq->elSize;
        memcpy(pq->buffer, last, pq->elSize);
        heapifyDown(pq, 0);
    }

    return ZZ_OK();
}

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
zzOpResult zzPriorityQueuePeek(const zzPriorityQueue *pq, void *out) {
    if (!pq) return ZZ_ERR("PriorityQueue pointer is NULL");
    if (!out) return ZZ_ERR("Output buffer is NULL");
    if (pq->size == 0) return ZZ_ERR("Queue is empty");

    memcpy(out, pq->buffer, pq->elSize);
    return ZZ_OK();
}

/**
 * @brief Clears all elements from the PriorityQueue.
 *
 * This function removes all elements from the priority queue by calling the custom free
 * function on each element (if provided) and resetting the size to zero.
 * The underlying buffer remains allocated with the same capacity.
 *
 * @param[in,out] pq Pointer to the PriorityQueue to clear
 */
void zzPriorityQueueClear(zzPriorityQueue *pq) {
    if (!pq) return;
    
    if (pq->elemFree) {
        for (size_t i = 0; i < pq->size; i++) {
            void *elem = (char*)pq->buffer + i * pq->elSize;
            pq->elemFree(elem);
        }
    }
    pq->size = 0;
}

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
void zzPriorityQueueIteratorInit(zzPriorityQueueIterator *it, zzPriorityQueue *pq) {
    if (!it || !pq) return;
    
    it->queue = pq;
    it->index = 0;
    it->state = (pq->size > 0) ? ZZ_ITER_VALID : ZZ_ITER_END;
}

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
bool zzPriorityQueueIteratorNext(zzPriorityQueueIterator *it, void *valueOut) {
    if (!it || !valueOut || it->state != ZZ_ITER_VALID) return false;
    
    if (it->index >= it->queue->size) {
        it->state = ZZ_ITER_END;
        return false;
    }
    
    void *elem = (char*)it->queue->buffer + it->index * it->queue->elSize;
    memcpy(valueOut, elem, it->queue->elSize);
    
    it->index++;
    
    return true;
}

/**
 * @brief Checks if the iterator has more elements.
 *
 * This function checks whether the iterator can advance to another element
 * without actually advancing it.
 *
 * @param[in] it Pointer to the iterator to check
 * @return true if there are more elements, false otherwise
 */
bool zzPriorityQueueIteratorHasNext(const zzPriorityQueueIterator *it) {
    return it && it->state == ZZ_ITER_VALID && it->index < it->queue->size;
}

/**
 * @brief Removes the last element returned by the iterator.
 *
 * This function removes the element that was most recently returned by
 * zzPriorityQueueIteratorNext. After removal, the iterator remains valid.
 * Note: Removal may affect the order of remaining elements in the iteration
 * due to heap restructuring.
 *
 * @param[in,out] it Pointer to the iterator
 * @return zzOpResult with status ZZ_SUCCESS on success, or ZZ_ERROR with error message on failure
 */
zzOpResult zzPriorityQueueIteratorRemove(zzPriorityQueueIterator *it) {
    if (!it || it->state != ZZ_ITER_VALID) return ZZ_ERR("Invalid iterator state");
    if (it->index == 0) return ZZ_ERR("No element to remove (Next not called or at start)");

    size_t removeIdx = it->index - 1;
    zzPriorityQueue *pq = it->queue;
    
    if (removeIdx >= pq->size) return ZZ_ERR("Index out of bounds");

    void *target = (char*)pq->buffer + removeIdx * pq->elSize;
    if (pq->elemFree) {
        pq->elemFree(target);
    }

    size_t lastIdx = pq->size - 1;
    if (removeIdx != lastIdx) {
        void *last = (char*)pq->buffer + lastIdx * pq->elSize;
        memcpy(target, last, pq->elSize);
        
        pq->size--; 
        
        heapifyDown(pq, removeIdx);
        heapifyUp(pq, removeIdx);
    } else {
        pq->size--;
    }

    it->index--; 

    if (it->index >= pq->size && pq->size == 0) {
        it->state = ZZ_ITER_END;
    } else if (it->index >= pq->size) {
        it->state = ZZ_ITER_END;
    }
    
    return ZZ_OK();
}