#include "circularBuffer.h"
#include <string.h>
#include <stdlib.h>

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

zzOpResult zzCircularBufferGet(const zzCircularBuffer *cb, size_t idx, void *out) {
    if (!cb) return ZZ_ERR("CircularBuffer pointer is NULL");
    if (!out) return ZZ_ERR("Output buffer is NULL");
    if (idx >= cb->size) return ZZ_ERR("Index out of bounds");

    size_t realIdx = (cb->head + idx) % cb->capacity;
    memcpy(out, (char*)cb->buffer + realIdx * cb->elSize, cb->elSize);
    return ZZ_OK();
}

zzOpResult zzCircularBufferPeekFront(const zzCircularBuffer *cb, void *out) {
    if (!cb) return ZZ_ERR("CircularBuffer pointer is NULL");
    if (!out) return ZZ_ERR("Output buffer is NULL");
    if (cb->size == 0) return ZZ_ERR("Buffer is empty");

    memcpy(out, (char*)cb->buffer + cb->head * cb->elSize, cb->elSize);
    return ZZ_OK();
}

zzOpResult zzCircularBufferPeekBack(const zzCircularBuffer *cb, void *out) {
    if (!cb) return ZZ_ERR("CircularBuffer pointer is NULL");
    if (!out) return ZZ_ERR("Output buffer is NULL");
    if (cb->size == 0) return ZZ_ERR("Buffer is empty");

    size_t idx = (cb->tail + cb->capacity - 1) % cb->capacity;
    memcpy(out, (char*)cb->buffer + idx * cb->elSize, cb->elSize);
    return ZZ_OK();
}

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
void zzCircularBufferIteratorInit(zzCircularBufferIterator *it, const zzCircularBuffer *cb) {
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
    if (it->index >= it->buffer->size) {
        it->state = ZZ_ITER_END;
    }
    
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
