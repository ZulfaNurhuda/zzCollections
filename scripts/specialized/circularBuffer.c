#include "circularBuffer.h"
#include <string.h>
#include <stdlib.h>

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
zzOpResult zzCircularBufferInit(zzCircularBuffer *cb, size_t elSize, size_t capacity, zzFreeFn elemFree) {
    if (!cb) return ZZ_ERR("CircularBuffer pointer is NULL");
    if (elSize == 0) return ZZ_ERR("Element size cannot be zero");
    if (capacity == 0) return ZZ_ERR("Capacity cannot be zero");

    cb->buffer = malloc(elSize * capacity);
    if (!cb->buffer) return ZZ_ERR("Failed to allocate buffer memory");

    cb->elSize = elSize;
    cb->capacity = capacity;
    cb->head = 0;
    cb->tail = 0;
    cb->size = 0;
    cb->elemFree = elemFree;
    return ZZ_OK();
}

/**
 * @brief Frees all resources associated with the CircularBuffer.
 *
 * This function releases all memory used by the CircularBuffer, including calling
 * the custom free function for each element if provided. After this function
 * returns, the CircularBuffer structure should not be used until reinitialized.
 *
 * @param[in,out] cb Pointer to the CircularBuffer to free
 */
void zzCircularBufferFree(zzCircularBuffer *cb) {
    if (!cb || !cb->buffer) return;

    if (cb->elemFree) {
        for (size_t i = 0; i < cb->size; i++) {
            size_t idx = (cb->head + i) % cb->capacity;
            void *elem = (char*)cb->buffer + idx * cb->elSize;
            cb->elemFree(elem);
        }
    }
    free(cb->buffer);
    cb->buffer = NULL;
}

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
zzOpResult zzCircularBufferPush(zzCircularBuffer *cb, const void *elem) {
    if (!cb) return ZZ_ERR("CircularBuffer pointer is NULL");
    if (!elem) return ZZ_ERR("Element pointer is NULL");

    void *target = (char*)cb->buffer + cb->tail * cb->elSize;

    if (cb->size == cb->capacity) {
        if (cb->elemFree) cb->elemFree(target);
        cb->head = (cb->head + 1) % cb->capacity;
    } else {
        cb->size++;
    }

    memcpy(target, elem, cb->elSize);
    cb->tail = (cb->tail + 1) % cb->capacity;
    return ZZ_OK();
}

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
zzOpResult zzCircularBufferPop(zzCircularBuffer *cb, void *out) {
    if (!cb) return ZZ_ERR("CircularBuffer pointer is NULL");
    if (!out) return ZZ_ERR("Output buffer is NULL");
    if (cb->size == 0) return ZZ_ERR("Buffer is empty");

    void *elem = (char*)cb->buffer + cb->head * cb->elSize;
    memcpy(out, elem, cb->elSize);

    cb->head = (cb->head + 1) % cb->capacity;
    cb->size--;
    return ZZ_OK();
}

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
zzOpResult zzCircularBufferGet(const zzCircularBuffer *cb, size_t idx, void *out) {
    if (!cb) return ZZ_ERR("CircularBuffer pointer is NULL");
    if (!out) return ZZ_ERR("Output buffer is NULL");
    if (idx >= cb->size) return ZZ_ERR("Index out of bounds");

    size_t realIdx = (cb->head + idx) % cb->capacity;
    memcpy(out, (char*)cb->buffer + realIdx * cb->elSize, cb->elSize);
    return ZZ_OK();
}

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
zzOpResult zzCircularBufferPeekFront(const zzCircularBuffer *cb, void *out) {
    if (!cb) return ZZ_ERR("CircularBuffer pointer is NULL");
    if (!out) return ZZ_ERR("Output buffer is NULL");
    if (cb->size == 0) return ZZ_ERR("Buffer is empty");

    memcpy(out, (char*)cb->buffer + cb->head * cb->elSize, cb->elSize);
    return ZZ_OK();
}

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
zzOpResult zzCircularBufferPeekBack(const zzCircularBuffer *cb, void *out) {
    if (!cb) return ZZ_ERR("CircularBuffer pointer is NULL");
    if (!out) return ZZ_ERR("Output buffer is NULL");
    if (cb->size == 0) return ZZ_ERR("Buffer is empty");

    size_t idx = (cb->tail + cb->capacity - 1) % cb->capacity;
    memcpy(out, (char*)cb->buffer + idx * cb->elSize, cb->elSize);
    return ZZ_OK();
}

/**
 * @brief Clears all elements from the CircularBuffer.
 *
 * This function removes all elements from the buffer by calling the custom free
 * function on each element (if provided) and resetting the size to zero.
 * The underlying buffer remains allocated with the same capacity.
 *
 * @param[in,out] cb Pointer to the CircularBuffer to clear
 */
void zzCircularBufferClear(zzCircularBuffer *cb) {
    if (!cb) return;

    if (cb->elemFree) {
        for (size_t i = 0; i < cb->size; i++) {
            size_t idx = (cb->head + i) % cb->capacity;
            void *elem = (char*)cb->buffer + idx * cb->elSize;
            cb->elemFree(elem);
        }
    }
    cb->head = 0;
    cb->tail = 0;
    cb->size = 0;
}

/**
 * @brief Initializes an iterator for the CircularBuffer.
 *
 * This function initializes an iterator to traverse the CircularBuffer from
 * the oldest element (head) to the newest element (tail).
 *
 * @param[out] it Pointer to the iterator structure to initialize
 * @param[in] cb Pointer to the CircularBuffer to iterate over
 */
void zzCircularBufferIteratorInit(zzCircularBufferIterator *it, zzCircularBuffer *cb) {
    if (!it || !cb) return;
    
    it->buffer = cb;
    it->index = 0;
    it->state = (cb->size > 0) ? ZZ_ITER_VALID : ZZ_ITER_END;
}

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
bool zzCircularBufferIteratorNext(zzCircularBufferIterator *it, void *valueOut) {
    if (!it || !valueOut || it->state != ZZ_ITER_VALID) return false;
    
    if (it->index >= it->buffer->size) {
        it->state = ZZ_ITER_END;
        return false;
    }
    
    size_t physicalIdx = (it->buffer->head + it->index) % it->buffer->capacity;
    void *elem = (char*)it->buffer->buffer + physicalIdx * it->buffer->elSize;
    memcpy(valueOut, elem, it->buffer->elSize);
    
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
bool zzCircularBufferIteratorHasNext(const zzCircularBufferIterator *it) {
    return it && it->state == ZZ_ITER_VALID && it->index < it->buffer->size;
}

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
zzOpResult zzCircularBufferIteratorRemove(zzCircularBufferIterator *it) {
    if (!it || it->state != ZZ_ITER_VALID) return ZZ_ERR("Invalid iterator state");
    if (it->index == 0) return ZZ_ERR("No element to remove (Next not called or at start)");

    size_t removeIdx = it->index - 1;
    zzCircularBuffer *cb = it->buffer;
    
    if (removeIdx >= cb->size) return ZZ_ERR("Index out of bounds");

    size_t physicalRemoveIdx = (cb->head + removeIdx) % cb->capacity;
    void *target = (char*)cb->buffer + physicalRemoveIdx * cb->elSize;

    if (cb->elemFree) {
        cb->elemFree(target);
    }

    for (size_t i = removeIdx; i < cb->size - 1; i++) {
        size_t currIdx = (cb->head + i) % cb->capacity;
        size_t nextIdx = (cb->head + i + 1) % cb->capacity;
        
        void *dst = (char*)cb->buffer + currIdx * cb->elSize;
        void *src = (char*)cb->buffer + nextIdx * cb->elSize;
        memcpy(dst, src, cb->elSize);
    }

    cb->size--;
    cb->tail = (cb->head + cb->size) % cb->capacity;
    it->index--; 

    if (it->index >= cb->size && cb->size == 0) {
        it->state = ZZ_ITER_END;
    } else if (it->index >= cb->size) {
        it->state = ZZ_ITER_END;
    }
    
    return ZZ_OK();
}