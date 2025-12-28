#include "arrayDeque.h"
#include "memory.h"
#include <stdlib.h>

bool zzArrayDequeInit(zzArrayDeque *ad, size_t elSize, size_t capacity, zzFreeFn elemFree) {
    if (!ad || elSize == 0) return false;
    if (capacity == 0) capacity = 4;
    
    ad->elSize = elSize;
    ad->capacity = capacity;
    ad->front = 0;
    ad->size = 0;
    ad->elemFree = elemFree;
    ad->buffer = malloc(elSize * capacity);
    return ad->buffer != NULL;
}

void zzArrayDequeFree(zzArrayDeque *ad) {
    if (!ad || !ad->buffer) return;
    
    if (ad->elemFree) {
        for (size_t i = 0; i < ad->size; i++) {
            size_t idx = (ad->front + i) % ad->capacity;
            void *elem = (char*)ad->buffer + idx * ad->elSize;
            ad->elemFree(elem);
        }
    }
    free(ad->buffer);
    ad->buffer = NULL;
}

static bool zzArrayDequeResize(zzArrayDeque *ad, size_t newCap) {
    void *newBuf = malloc(newCap * ad->elSize);
    if (!newBuf) return false;
    
    for (size_t i = 0; i < ad->size; i++) {
        size_t oldIdx = (ad->front + i) % ad->capacity;
        zzMemoryCopy((char*)newBuf + i * ad->elSize,
                   (char*)ad->buffer + oldIdx * ad->elSize,
                   ad->elSize);
    }
    
    free(ad->buffer);
    ad->buffer = newBuf;
    ad->capacity = newCap;
    ad->front = 0;
    return true;
}

bool zzArrayDequePushFront(zzArrayDeque *ad, const void *elem) {
    if (!ad || !elem) return false;
    if (ad->size == ad->capacity && !zzArrayDequeResize(ad, ad->capacity * 2))
        return false;
    
    ad->front = (ad->front + ad->capacity - 1) % ad->capacity;
    zzMemoryCopy((char*)ad->buffer + ad->front * ad->elSize, elem, ad->elSize);
    ad->size++;
    return true;
}

bool zzArrayDequePushBack(zzArrayDeque *ad, const void *elem) {
    if (!ad || !elem) return false;
    if (ad->size == ad->capacity && !zzArrayDequeResize(ad, ad->capacity * 2))
        return false;
    
    size_t idx = (ad->front + ad->size) % ad->capacity;
    zzMemoryCopy((char*)ad->buffer + idx * ad->elSize, elem, ad->elSize);
    ad->size++;
    return true;
}

bool zzArrayDequePopFront(zzArrayDeque *ad, void *out) {
    if (!ad || ad->size == 0) return false;
    
    void *elem = (char*)ad->buffer + ad->front * ad->elSize;
    if (out) zzMemoryCopy(out, elem, ad->elSize);
    if (ad->elemFree && !out) ad->elemFree(elem);
    
    ad->front = (ad->front + 1) % ad->capacity;
    ad->size--;
    return true;
}

bool zzArrayDequePopBack(zzArrayDeque *ad, void *out) {
    if (!ad || ad->size == 0) return false;
    
    size_t idx = (ad->front + ad->size - 1) % ad->capacity;
    void *elem = (char*)ad->buffer + idx * ad->elSize;
    
    if (out) zzMemoryCopy(out, elem, ad->elSize);
    if (ad->elemFree && !out) ad->elemFree(elem);
    
    ad->size--;
    return true;
}

bool zzArrayDequePeekFront(const zzArrayDeque *ad, void *out) {
    if (!ad || !out || ad->size == 0) return false;
    zzMemoryCopy(out, (char*)ad->buffer + ad->front * ad->elSize, ad->elSize);
    return true;
}

bool zzArrayDequePeekBack(const zzArrayDeque *ad, void *out) {
    if (!ad || !out || ad->size == 0) return false;
    size_t idx = (ad->front + ad->size - 1) % ad->capacity;
    zzMemoryCopy(out, (char*)ad->buffer + idx * ad->elSize, ad->elSize);
    return true;
}

bool zzArrayDequeGet(const zzArrayDeque *ad, size_t idx, void *out) {
    if (!ad || !out || idx >= ad->size) return false;
    size_t realIdx = (ad->front + idx) % ad->capacity;
    zzMemoryCopy(out, (char*)ad->buffer + realIdx * ad->elSize, ad->elSize);
    return true;
}

void zzArrayDequeClear(zzArrayDeque *ad) {
    if (!ad) return;
    
    if (ad->elemFree) {
        for (size_t i = 0; i < ad->size; i++) {
            size_t idx = (ad->front + i) % ad->capacity;
            void *elem = (char*)ad->buffer + idx * ad->elSize;
            ad->elemFree(elem);
        }
    }
    ad->size = 0;
    ad->front = 0;
}
