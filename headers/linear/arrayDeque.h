#ifndef ARRAY_DEQUE_H
#define ARRAY_DEQUE_H

#include "types.h"
#include "utils.h"
#include "result.h"

typedef struct zzArrayDeque {
    void *buffer;
    size_t capacity;
    size_t front;
    size_t size;
    size_t elSize;
    zzFreeFn elemFree;
} zzArrayDeque;

zzOpResult zzArrayDequeInit(zzArrayDeque *ad, size_t elSize, size_t capacity, zzFreeFn elemFree);

void zzArrayDequeFree(zzArrayDeque *ad);

zzOpResult zzArrayDequePushFront(zzArrayDeque *ad, const void *elem);

zzOpResult zzArrayDequePushBack(zzArrayDeque *ad, const void *elem);

zzOpResult zzArrayDequePopFront(zzArrayDeque *ad, void *out);

zzOpResult zzArrayDequePopBack(zzArrayDeque *ad, void *out);

zzOpResult zzArrayDequePeekFront(const zzArrayDeque *ad, void *out);

zzOpResult zzArrayDequePeekBack(const zzArrayDeque *ad, void *out);

zzOpResult zzArrayDequeGet(const zzArrayDeque *ad, size_t idx, void *out);

void zzArrayDequeClear(zzArrayDeque *ad);

#endif