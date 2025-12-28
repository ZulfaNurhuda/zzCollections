#ifndef ARRAY_QUEUE_H
#define ARRAY_QUEUE_H

#include "arrayDeque.h"

typedef zzArrayDeque zzArrayQueue;

static inline bool zzArrayQueueInit(zzArrayQueue *q, size_t elSize, size_t capacity, zzFreeFn elemFree) {
    return zzArrayDequeInit(q, elSize, capacity, elemFree);
}

static inline void zzArrayQueueFree(zzArrayQueue *q) {
    zzArrayDequeFree(q);
}

static inline bool zzArrayQueueEnqueue(zzArrayQueue *q, const void *elem) {
    return zzArrayDequePushBack(q, elem);
}

static inline bool zzArrayQueueDequeue(zzArrayQueue *q, void *out) {
    return zzArrayDequePopFront(q, out);
}

static inline bool zzArrayQueuePeek(const zzArrayQueue *q, void *out) {
    return zzArrayDequePeekFront(q, out);
}

static inline bool zzArrayQueueIsEmpty(const zzArrayQueue *q) {
    return q->size == 0;
}

static inline size_t zzArrayQueueSize(const zzArrayQueue *q) {
    return q->size;
}

static inline void zzArrayQueueClear(zzArrayQueue *q) {
    zzArrayDequeClear(q);
}

#endif
