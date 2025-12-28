#ifndef LINKED_QUEUE_H
#define LINKED_QUEUE_H

#include "linkedList.h"
#include "memory.h"

typedef zzLinkedList zzLinkedQueue;

static inline bool zzLinkedQueueInit(zzLinkedQueue *q, size_t elSize, zzFreeFn elemFree) {
    return zzLinkedListInit(q, elSize, elemFree);
}

static inline void zzLinkedQueueFree(zzLinkedQueue *q) {
    zzLinkedListFree(q);
}

static inline bool zzLinkedQueueEnqueue(zzLinkedQueue *q, const void *elem) {
    return zzLinkedListPushBack(q, elem);
}

static inline bool zzLinkedQueueDequeue(zzLinkedQueue *q, void *out) {
    return zzLinkedListPopFront(q, out);
}

static inline bool zzLinkedQueuePeek(const zzLinkedQueue *q, void *out) {
    if (!q || !out || !q->head) return false;
    zzMemoryCopy(out, q->head->data, q->elSize);
    return true;
}

static inline bool zzLinkedQueueIsEmpty(const zzLinkedQueue *q) {
    return q->size == 0;
}

static inline size_t zzLinkedQueueSize(const zzLinkedQueue *q) {
    return q->size;
}

static inline void zzLinkedQueueClear(zzLinkedQueue *q) {
    zzLinkedListClear(q);
}

#endif
