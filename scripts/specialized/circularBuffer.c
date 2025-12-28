#include "circularBuffer.h"
#include "memory.h"
#include <stdlib.h>

bool zzCircularBufferInit(zzCircularBuffer *cb, size_t elSize, size_t capacity, zzFreeFn elemFree) {
    if (!cb || elSize == 0 || capacity == 0) return false;
    
    cb->buffer = malloc(elSize * capacity);
    if (!cb->buffer) return false;
    
    cb->elSize = elSize;
    cb->capacity = capacity;
    cb->head = 0;
    cb->tail = 0;
    cb->size = 0;
    cb->elemFree = elemFree;
    return true;
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

bool zzCircularBufferPush(zzCircularBuffer *cb, const void *elem) {
    if (!cb || !elem) return false;
    
    void *target = (char*)cb->buffer + cb->tail * cb->elSize;
    
    if (cb->size == cb->capacity) {
        if (cb->elemFree) cb->elemFree(target);
        cb->head = (cb->head + 1) % cb->capacity;
    } else {
        cb->size++;
    }
    
    zzMemoryCopy(target, elem, cb->elSize);
    cb->tail = (cb->tail + 1) % cb->capacity;
    return true;
}

bool zzCircularBufferPop(zzCircularBuffer *cb, void *out) {
    if (!cb || cb->size == 0) return false;
    
    void *elem = (char*)cb->buffer + cb->head * cb->elSize;
    if (out) zzMemoryCopy(out, elem, cb->elSize);
    if (cb->elemFree && !out) cb->elemFree(elem);
    
    cb->head = (cb->head + 1) % cb->capacity;
    cb->size--;
    return true;
}

bool zzCircularBufferGet(const zzCircularBuffer *cb, size_t idx, void *out) {
    if (!cb || !out || idx >= cb->size) return false;
    size_t realIdx = (cb->head + idx) % cb->capacity;
    zzMemoryCopy(out, (char*)cb->buffer + realIdx * cb->elSize, cb->elSize);
    return true;
}

bool zzCircularBufferPeekFront(const zzCircularBuffer *cb, void *out) {
    if (!cb || !out || cb->size == 0) return false;
    zzMemoryCopy(out, (char*)cb->buffer + cb->head * cb->elSize, cb->elSize);
    return true;
}

bool zzCircularBufferPeekBack(const zzCircularBuffer *cb, void *out) {
    if (!cb || !out || cb->size == 0) return false;
    size_t idx = (cb->tail + cb->capacity - 1) % cb->capacity;
    zzMemoryCopy(out, (char*)cb->buffer + idx * cb->elSize, cb->elSize);
    return true;
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
