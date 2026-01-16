#ifndef LINKED_QUEUE_H
#define LINKED_QUEUE_H

#include "linkedList.h"
#include <string.h>

typedef zzLinkedList zzLinkedQueue;

static inline zzOpResult zzLinkedQueueInit(zzLinkedQueue *q, size_t elSize, zzFreeFn elemFree) {
    return zzLinkedListInit(q, elSize, elemFree);
}

static inline void zzLinkedQueueFree(zzLinkedQueue *q) {
    zzLinkedListFree(q);
}

static inline zzOpResult zzLinkedQueueEnqueue(zzLinkedQueue *q, const void *elem) {
    return zzLinkedListPushBack(q, elem);
}

static inline zzOpResult zzLinkedQueueDequeue(zzLinkedQueue *q, void *out) {
    return zzLinkedListPopFront(q, out);
}

static inline zzOpResult zzLinkedQueuePeek(const zzLinkedQueue *q, void *out) {
    if (!q) return ZZ_ERR("LinkedList pointer is NULL");
    if (!out) return ZZ_ERR("Output buffer is NULL");
    if (!q->head) return ZZ_ERR("List is empty");

    memcpy(out, q->head->data, q->elSize);
    return ZZ_OK();
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