#ifndef CIRCULAR_BUFFER_H
#define CIRCULAR_BUFFER_H

#include "types.h"
#include "utils.h"
#include "result.h"

typedef struct zzCircularBuffer {
    void *buffer;
    size_t elSize;
    size_t capacity;
    size_t head;
    size_t tail;
    size_t size;
    zzFreeFn elemFree;
} zzCircularBuffer;

zzOpResult zzCircularBufferInit(zzCircularBuffer *cb, size_t elSize, size_t capacity, zzFreeFn elemFree);

void zzCircularBufferFree(zzCircularBuffer *cb);

zzOpResult zzCircularBufferPush(zzCircularBuffer *cb, const void *elem);

zzOpResult zzCircularBufferPop(zzCircularBuffer *cb, void *out);

zzOpResult zzCircularBufferGet(const zzCircularBuffer *cb, size_t idx, void *out);

zzOpResult zzCircularBufferPeekFront(const zzCircularBuffer *cb, void *out);

zzOpResult zzCircularBufferPeekBack(const zzCircularBuffer *cb, void *out);

void zzCircularBufferClear(zzCircularBuffer *cb);

#endif