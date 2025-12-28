#include "priorityQueue.h"
#include "memory.h"
#include <stdlib.h>

bool zzPriorityQueueInit(zzPriorityQueue *pq, size_t elSize, size_t capacity,
                       zzCompareFn compareFn, zzFreeFn elemFree) {
    if (!pq || elSize == 0 || !compareFn) return false;
    if (capacity == 0) capacity = 16;
    
    pq->buffer = malloc(elSize * capacity);
    if (!pq->buffer) return false;
    
    pq->elSize = elSize;
    pq->size = 0;
    pq->capacity = capacity;
    pq->compareFn = compareFn;
    pq->elemFree = elemFree;
    return true;
}

void zzPriorityQueueFree(zzPriorityQueue *pq) {
    if (!pq || !pq->buffer) return;
    
    if (pq->elemFree) {
        for (size_t i = 0; i < pq->size; i++) {
            void *elem = (char*)pq->buffer + i * pq->elSize;
            pq->elemFree(elem);
        }
    }
    free(pq->buffer);
    pq->buffer = NULL;
    pq->size = 0;
}

static bool zzPriorityQueueResize(zzPriorityQueue *pq) {
    size_t newCap = pq->capacity * 2;
    void *newBuf = realloc(pq->buffer, pq->elSize * newCap);
    if (!newBuf) return false;
    pq->buffer = newBuf;
    pq->capacity = newCap;
    return true;
}

static void heapifyUp(zzPriorityQueue *pq, size_t idx) {
    while (idx > 0) {
        size_t parent = (idx - 1) / 2;
        void *child = (char*)pq->buffer + idx * pq->elSize;
        void *parentElem = (char*)pq->buffer + parent * pq->elSize;
        
        if (pq->compareFn(child, parentElem) >= 0) break;
        
        char tmp[pq->elSize];
        zzMemoryCopy(tmp, child, pq->elSize);
        zzMemoryCopy(child, parentElem, pq->elSize);
        zzMemoryCopy(parentElem, tmp, pq->elSize);
        
        idx = parent;
    }
}

static void heapifyDown(zzPriorityQueue *pq, size_t idx) {
    while (1) {
        size_t smallest = idx;
        size_t left = 2 * idx + 1;
        size_t right = 2 * idx + 2;
        
        if (left < pq->size) {
            void *leftElem = (char*)pq->buffer + left * pq->elSize;
            void *smallestElem = (char*)pq->buffer + smallest * pq->elSize;
            if (pq->compareFn(leftElem, smallestElem) < 0) {
                smallest = left;
            }
        }
        
        if (right < pq->size) {
            void *rightElem = (char*)pq->buffer + right * pq->elSize;
            void *smallestElem = (char*)pq->buffer + smallest * pq->elSize;
            if (pq->compareFn(rightElem, smallestElem) < 0) {
                smallest = right;
            }
        }
        
        if (smallest == idx) break;
        
        void *idxElem = (char*)pq->buffer + idx * pq->elSize;
        void *smallestElem = (char*)pq->buffer + smallest * pq->elSize;
        
        char tmp[pq->elSize];
        zzMemoryCopy(tmp, idxElem, pq->elSize);
        zzMemoryCopy(idxElem, smallestElem, pq->elSize);
        zzMemoryCopy(smallestElem, tmp, pq->elSize);
        
        idx = smallest;
    }
}

bool zzPriorityQueuePush(zzPriorityQueue *pq, const void *elem) {
    if (!pq || !elem) return false;
    if (pq->size == pq->capacity && !zzPriorityQueueResize(pq)) return false;
    
    void *target = (char*)pq->buffer + pq->size * pq->elSize;
    zzMemoryCopy(target, elem, pq->elSize);
    heapifyUp(pq, pq->size);
    pq->size++;
    return true;
}

bool zzPriorityQueuePop(zzPriorityQueue *pq, void *out) {
    if (!pq || pq->size == 0) return false;
    
    if (out) {
        zzMemoryCopy(out, pq->buffer, pq->elSize);
    }
    
    pq->size--;
    if (pq->size > 0) {
        void *last = (char*)pq->buffer + pq->size * pq->elSize;
        zzMemoryCopy(pq->buffer, last, pq->elSize);
        heapifyDown(pq, 0);
    }
    
    return true;
}

bool zzPriorityQueuePeek(const zzPriorityQueue *pq, void *out) {
    if (!pq || !out || pq->size == 0) return false;
    zzMemoryCopy(out, pq->buffer, pq->elSize);
    return true;
}

void zzPriorityQueueClear(zzPriorityQueue *pq) {
    if (!pq) return;
    
    if (pq->elemFree) {
        for (size_t i = 0; i < pq->size; i++) {
            void *elem = (char*)pq->buffer + i * pq->elSize;
            pq->elemFree(elem);
        }
    }
    pq->size = 0;
}
