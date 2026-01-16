#ifndef PRIORITY_QUEUE_H
#define PRIORITY_QUEUE_H

#include "types.h"
#include "utils.h"
#include "result.h"

typedef struct zzPriorityQueue {
    void *buffer;
    size_t elSize;
    size_t size;
    size_t capacity;
    zzCompareFn compareFn;
    zzFreeFn elemFree;
} zzPriorityQueue;

zzOpResult zzPriorityQueueInit(zzPriorityQueue *pq, size_t elSize, size_t capacity, zzCompareFn compareFn, zzFreeFn elemFree);

void zzPriorityQueueFree(zzPriorityQueue *pq);

zzOpResult zzPriorityQueuePush(zzPriorityQueue *pq, const void *elem);

zzOpResult zzPriorityQueuePop(zzPriorityQueue *pq, void *out);

zzOpResult zzPriorityQueuePeek(const zzPriorityQueue *pq, void *out);

void zzPriorityQueueClear(zzPriorityQueue *pq);

#endif