#ifndef PRIORITY_QUEUE_H
#define PRIORITY_QUEUE_H

#include "common.h"

typedef struct {
    void *buffer;
    size_t elSize;
    size_t size;
    size_t capacity;
    zzCompareFn compareFn;
    zzFreeFn elemFree;
} zzPriorityQueue;

bool zzPriorityQueueInit(zzPriorityQueue *pq, size_t elSize, size_t capacity,
                       zzCompareFn compareFn, zzFreeFn elemFree);
void zzPriorityQueueFree(zzPriorityQueue *pq);
bool zzPriorityQueuePush(zzPriorityQueue *pq, const void *elem);
bool zzPriorityQueuePop(zzPriorityQueue *pq, void *out);
bool zzPriorityQueuePeek(const zzPriorityQueue *pq, void *out);
void zzPriorityQueueClear(zzPriorityQueue *pq);

#endif
