/**
 * @file arrayDeque.c
 * @brief Implementation of the circular buffer-based double-ended queue (deque) data structure.
 *
 * This module provides the implementation for the ArrayDeque data structure,
 * which offers efficient double-ended queue functionality using a circular buffer
 * approach. The implementation handles memory management and provides O(1)
 * operations for adding and removing elements from both ends.
 */

#include "arrayDeque.h"
#include <string.h>
#include <stdlib.h>

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
zzOpResult zzArrayDequeInit(zzArrayDeque *ad, size_t elSize, size_t capacity, zzFreeFn elemFree) {
    if (!ad) return ZZ_ERR("ArrayDeque pointer is NULL");
    if (elSize == 0) return ZZ_ERR("Element size cannot be zero");
    if (capacity == 0) capacity = 4;

    ad->elSize = elSize;
    ad->capacity = capacity;
    ad->front = 0;
    ad->size = 0;
    ad->elemFree = elemFree;
    ad->buffer = malloc(elSize * capacity);

    if (!ad->buffer) return ZZ_ERR("Failed to allocate buffer memory");
    return ZZ_OK();
}

/**
 * @brief Frees all resources associated with the ArrayDeque.
 *
 * This function releases all memory used by the ArrayDeque, including calling
 * the custom free function for each element if provided. After this function
 * returns, the ArrayDeque structure should not be used until reinitialized.
 *
 * @param[in,out] ad Pointer to the ArrayDeque to free
 */
void zzArrayDequeFree(zzArrayDeque *ad) {
    if (!ad || !ad->buffer) return;

    if (ad->elemFree) {
        for (size_t i = 0; i < ad->size; i++) {
            size_t idx = (ad->front + i) % ad->capacity;
            void *elem = (char*)ad->buffer + idx * ad->elSize;
            ad->elemFree(elem);
        }
    }
    free(ad->buffer);
    ad->buffer = NULL;
}

/**
 * @brief Internal function to resize the ArrayDeque buffer when capacity is exceeded.
 *
 * This helper function increases the capacity of the ArrayDeque to the specified
 * size and reorganizes the elements to maintain proper ordering in the new buffer.
 * The elements are copied from the old circular buffer to the new one with the
 * front element at index 0.
 *
 * @param[in,out] ad Pointer to the ArrayDeque to resize
 * @param[in] newCap New capacity for the ArrayDeque
 * @return zzOpResult with status ZZ_SUCCESS on success, or ZZ_ERROR with error message on failure
 */
static zzOpResult zzArrayDequeResize(zzArrayDeque *ad, size_t newCap) {
    void *newBuf = malloc(newCap * ad->elSize);
    if (!newBuf) return ZZ_ERR("Failed to grow buffer (realloc failed)");

    for (size_t i = 0; i < ad->size; i++) {
        size_t oldIdx = (ad->front + i) % ad->capacity;
        memcpy((char*)newBuf + i * ad->elSize,
                     (char*)ad->buffer + oldIdx * ad->elSize,
                     ad->elSize);
    }

    free(ad->buffer);
    ad->buffer = newBuf;
    ad->capacity = newCap;
    ad->front = 0;
    return ZZ_OK();
}

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
zzOpResult zzArrayDequePushFront(zzArrayDeque *ad, const void *elem) {
    if (!ad) return ZZ_ERR("ArrayDeque pointer is NULL");
    if (!elem) return ZZ_ERR("Element pointer is NULL");

    if (ad->size == ad->capacity) {
        zzOpResult resizeResult = zzArrayDequeResize(ad, ad->capacity * 2);
        if (ZZ_IS_ERR(resizeResult)) return resizeResult;
    }

    ad->front = (ad->front + ad->capacity - 1) % ad->capacity;
    memcpy((char*)ad->buffer + ad->front * ad->elSize, elem, ad->elSize);
    ad->size++;
    return ZZ_OK();
}

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
zzOpResult zzArrayDequePushBack(zzArrayDeque *ad, const void *elem) {
    if (!ad) return ZZ_ERR("ArrayDeque pointer is NULL");
    if (!elem) return ZZ_ERR("Element pointer is NULL");

    if (ad->size == ad->capacity) {
        zzOpResult resizeResult = zzArrayDequeResize(ad, ad->capacity * 2);
        if (ZZ_IS_ERR(resizeResult)) return resizeResult;
    }

    size_t idx = (ad->front + ad->size) % ad->capacity;
    memcpy((char*)ad->buffer + idx * ad->elSize, elem, ad->elSize);
    ad->size++;
    return ZZ_OK();
}

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
zzOpResult zzArrayDequePopFront(zzArrayDeque *ad, void *out) {
    if (!ad) return ZZ_ERR("ArrayDeque pointer is NULL");
    if (!out) return ZZ_ERR("Output buffer is NULL");
    if (ad->size == 0) return ZZ_ERR("Deque is empty");

    void *elem = (char*)ad->buffer + ad->front * ad->elSize;
    memcpy(out, elem, ad->elSize);

    ad->front = (ad->front + 1) % ad->capacity;
    ad->size--;
    return ZZ_OK();
}

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
zzOpResult zzArrayDequePopBack(zzArrayDeque *ad, void *out) {
    if (!ad) return ZZ_ERR("ArrayDeque pointer is NULL");
    if (!out) return ZZ_ERR("Output buffer is NULL");
    if (ad->size == 0) return ZZ_ERR("Deque is empty");

    size_t idx = (ad->front + ad->size - 1) % ad->capacity;
    void *elem = (char*)ad->buffer + idx * ad->elSize;
    memcpy(out, elem, ad->elSize);

    ad->size--;
    return ZZ_OK();
}

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
zzOpResult zzArrayDequePeekFront(const zzArrayDeque *ad, void *out) {
    if (!ad) return ZZ_ERR("ArrayDeque pointer is NULL");
    if (!out) return ZZ_ERR("Output buffer is NULL");
    if (ad->size == 0) return ZZ_ERR("Deque is empty");

    memcpy(out, (char*)ad->buffer + ad->front * ad->elSize, ad->elSize);
    return ZZ_OK();
}

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
zzOpResult zzArrayDequePeekBack(const zzArrayDeque *ad, void *out) {
    if (!ad) return ZZ_ERR("ArrayDeque pointer is NULL");
    if (!out) return ZZ_ERR("Output buffer is NULL");
    if (ad->size == 0) return ZZ_ERR("Deque is empty");

    size_t idx = (ad->front + ad->size - 1) % ad->capacity;
    memcpy(out, (char*)ad->buffer + idx * ad->elSize, ad->elSize);
    return ZZ_OK();
}

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
zzOpResult zzArrayDequeGet(const zzArrayDeque *ad, size_t idx, void *out) {
    if (!ad) return ZZ_ERR("ArrayDeque pointer is NULL");
    if (!out) return ZZ_ERR("Output buffer is NULL");
    if (idx >= ad->size) return ZZ_ERR("Index out of bounds");

    size_t realIdx = (ad->front + idx) % ad->capacity;
    memcpy(out, (char*)ad->buffer + realIdx * ad->elSize, ad->elSize);
    return ZZ_OK();
}

/**
 * @brief Clears all elements from the ArrayDeque.
 *
 * This function removes all elements from the deque by calling the custom free
 * function on each element (if provided) and resetting the size to zero.
 * The underlying buffer remains allocated with the same capacity.
 *
 * @param[in,out] ad Pointer to the ArrayDeque to clear
 */
void zzArrayDequeClear(zzArrayDeque *ad) {
    if (!ad) return;

    if (ad->elemFree) {
        for (size_t i = 0; i < ad->size; i++) {
            size_t idx = (ad->front + i) % ad->capacity;
            void *elem = (char*)ad->buffer + idx * ad->elSize;
            ad->elemFree(elem);
        }
    }
    ad->size = 0;
    ad->front = 0;
}

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
void zzArrayDequeIteratorInit(zzArrayDequeIterator *it, zzArrayDeque *ad) {
    if (!it || !ad) return;
    
    it->deque = ad;
    it->index = 0;
    it->state = (ad->size > 0) ? ZZ_ITER_VALID : ZZ_ITER_END;
}

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
bool zzArrayDequeIteratorNext(zzArrayDequeIterator *it, void *valueOut) {
    if (!it || !valueOut || it->state != ZZ_ITER_VALID) return false;
    
    if (it->index >= it->deque->size) {
        it->state = ZZ_ITER_END;
        return false;
    }
    
    size_t physicalIdx = (it->deque->front + it->index) % it->deque->capacity;
    void *elem = (char*)it->deque->buffer + physicalIdx * it->deque->elSize;
    memcpy(valueOut, elem, it->deque->elSize);
    
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
bool zzArrayDequeIteratorHasNext(const zzArrayDequeIterator *it) {
    return it && it->state == ZZ_ITER_VALID && it->index < it->deque->size;
}

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
zzOpResult zzArrayDequeIteratorRemove(zzArrayDequeIterator *it) {
    if (!it || it->state != ZZ_ITER_VALID) return ZZ_ERR("Invalid iterator state");
    if (it->index == 0) return ZZ_ERR("No element to remove (Next not called or at start)");

    size_t removeIdx = it->index - 1;
    zzArrayDeque *ad = it->deque;
    
    if (removeIdx >= ad->size) return ZZ_ERR("Index out of bounds");

    size_t physicalRemoveIdx = (ad->front + removeIdx) % ad->capacity;
    void *target = (char*)ad->buffer + physicalRemoveIdx * ad->elSize;

    if (ad->elemFree) {
        ad->elemFree(target);
    }

    for (size_t i = removeIdx; i < ad->size - 1; i++) {
        size_t currIdx = (ad->front + i) % ad->capacity;
        size_t nextIdx = (ad->front + i + 1) % ad->capacity;
        
        void *dst = (char*)ad->buffer + currIdx * ad->elSize;
        void *src = (char*)ad->buffer + nextIdx * ad->elSize;
        memcpy(dst, src, ad->elSize);
    }

    ad->size--;
    it->index--; 

    if (it->index >= ad->size && ad->size == 0) {
        it->state = ZZ_ITER_END;
    } else if (it->index >= ad->size) {
        it->state = ZZ_ITER_END;
    }
    
    return ZZ_OK();
}