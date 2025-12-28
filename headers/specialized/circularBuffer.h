#ifndef CIRCULAR_BUFFER_H
#define CIRCULAR_BUFFER_H

#include "common.h"

typedef struct {
    void *buffer;
    size_t elSize;
    size_t capacity;
    size_t head;
    size_t tail;
    size_t size;
    zzFreeFn elemFree;
} zzCircularBuffer;

bool zzCircularBufferInit(zzCircularBuffer *cb, size_t elSize, size_t capacity, zzFreeFn elemFree);
void zzCircularBufferFree(zzCircularBuffer *cb);
bool zzCircularBufferPush(zzCircularBuffer *cb, const void *elem);
bool zzCircularBufferPop(zzCircularBuffer *cb, void *out);
bool zzCircularBufferGet(const zzCircularBuffer *cb, size_t idx, void *out);
bool zzCircularBufferPeekFront(const zzCircularBuffer *cb, void *out);
bool zzCircularBufferPeekBack(const zzCircularBuffer *cb, void *out);
void zzCircularBufferClear(zzCircularBuffer *cb);

#endif
