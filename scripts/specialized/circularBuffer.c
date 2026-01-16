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
