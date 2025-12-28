#include "arrayDeque.h"
#include "memory.h"
#include <stdlib.h>

zzOpResult zzArrayDequeInit(zzArrayDeque *ad, size_t elSize, size_t capacity, zzFreeFn elemFree) {
    if (!ad) return ZZ_ERR("ArrayDeque pointer is NULL");
    if (elSize == 0) return ZZ_ERR("Element size cannot be zero");
    if (capacity == 0) capacity = 4;

    ad->elSize = elSize;
    ad->capacity = capacity;
    ad->front = 0;
    ad->size = 0;
    ad->elemFree = elemFree;
    ad->buffer = malloc(elSize * capacity);

    if (!ad->buffer) return ZZ_ERR("Failed to allocate buffer memory");
    return ZZ_OK();
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

static zzOpResult zzArrayDequeResize(zzArrayDeque *ad, size_t newCap) {
    void *newBuf = malloc(newCap * ad->elSize);
    if (!newBuf) return ZZ_ERR("Failed to grow buffer (realloc failed)");

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
    return ZZ_OK();
}

zzOpResult zzArrayDequePushFront(zzArrayDeque *ad, const void *elem) {
    if (!ad) return ZZ_ERR("ArrayDeque pointer is NULL");
    if (!elem) return ZZ_ERR("Element pointer is NULL");

    if (ad->size == ad->capacity) {
        zzOpResult resizeResult = zzArrayDequeResize(ad, ad->capacity * 2);
        if (ZZ_IS_ERR(resizeResult)) return resizeResult;
    }

    ad->front = (ad->front + ad->capacity - 1) % ad->capacity;
    zzMemoryCopy((char*)ad->buffer + ad->front * ad->elSize, elem, ad->elSize);
    ad->size++;
    return ZZ_OK();
}

zzOpResult zzArrayDequePushBack(zzArrayDeque *ad, const void *elem) {
    if (!ad) return ZZ_ERR("ArrayDeque pointer is NULL");
    if (!elem) return ZZ_ERR("Element pointer is NULL");

    if (ad->size == ad->capacity) {
        zzOpResult resizeResult = zzArrayDequeResize(ad, ad->capacity * 2);
        if (ZZ_IS_ERR(resizeResult)) return resizeResult;
    }

    size_t idx = (ad->front + ad->size) % ad->capacity;
    zzMemoryCopy((char*)ad->buffer + idx * ad->elSize, elem, ad->elSize);
    ad->size++;
    return ZZ_OK();
}

zzOpResult zzArrayDequePopFront(zzArrayDeque *ad, void *out) {
    if (!ad) return ZZ_ERR("ArrayDeque pointer is NULL");
    if (!out) return ZZ_ERR("Output buffer is NULL");
    if (ad->size == 0) return ZZ_ERR("Deque is empty");

    void *elem = (char*)ad->buffer + ad->front * ad->elSize;
    zzMemoryCopy(out, elem, ad->elSize);

    ad->front = (ad->front + 1) % ad->capacity;
    ad->size--;
    return ZZ_OK();
}

zzOpResult zzArrayDequePopBack(zzArrayDeque *ad, void *out) {
    if (!ad) return ZZ_ERR("ArrayDeque pointer is NULL");
    if (!out) return ZZ_ERR("Output buffer is NULL");
    if (ad->size == 0) return ZZ_ERR("Deque is empty");

    size_t idx = (ad->front + ad->size - 1) % ad->capacity;
    void *elem = (char*)ad->buffer + idx * ad->elSize;
    zzMemoryCopy(out, elem, ad->elSize);

    ad->size--;
    return ZZ_OK();
}

zzOpResult zzArrayDequePeekFront(const zzArrayDeque *ad, void *out) {
    if (!ad) return ZZ_ERR("ArrayDeque pointer is NULL");
    if (!out) return ZZ_ERR("Output buffer is NULL");
    if (ad->size == 0) return ZZ_ERR("Deque is empty");

    zzMemoryCopy(out, (char*)ad->buffer + ad->front * ad->elSize, ad->elSize);
    return ZZ_OK();
}

zzOpResult zzArrayDequePeekBack(const zzArrayDeque *ad, void *out) {
    if (!ad) return ZZ_ERR("ArrayDeque pointer is NULL");
    if (!out) return ZZ_ERR("Output buffer is NULL");
    if (ad->size == 0) return ZZ_ERR("Deque is empty");

    size_t idx = (ad->front + ad->size - 1) % ad->capacity;
    zzMemoryCopy(out, (char*)ad->buffer + idx * ad->elSize, ad->elSize);
    return ZZ_OK();
}

zzOpResult zzArrayDequeGet(const zzArrayDeque *ad, size_t idx, void *out) {
    if (!ad) return ZZ_ERR("ArrayDeque pointer is NULL");
    if (!out) return ZZ_ERR("Output buffer is NULL");
    if (idx >= ad->size) return ZZ_ERR("Index out of bounds");

    size_t realIdx = (ad->front + idx) % ad->capacity;
    zzMemoryCopy(out, (char*)ad->buffer + realIdx * ad->elSize, ad->elSize);
    return ZZ_OK();
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
