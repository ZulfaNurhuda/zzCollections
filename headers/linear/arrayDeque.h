#ifndef ARRAY_DEQUE_H
#define ARRAY_DEQUE_H

#include "common.h"

typedef struct {
    void *buffer;
    size_t elSize;
    size_t capacity;
    size_t front;
    size_t size;
    zzFreeFn elemFree;
} zzArrayDeque;

bool zzArrayDequeInit(zzArrayDeque *ad, size_t elSize, size_t capacity, zzFreeFn elemFree);
void zzArrayDequeFree(zzArrayDeque *ad);
bool zzArrayDequePushFront(zzArrayDeque *ad, const void *elem);
bool zzArrayDequePushBack(zzArrayDeque *ad, const void *elem);
bool zzArrayDequePopFront(zzArrayDeque *ad, void *out);
bool zzArrayDequePopBack(zzArrayDeque *ad, void *out);
bool zzArrayDequePeekFront(const zzArrayDeque *ad, void *out);
bool zzArrayDequePeekBack(const zzArrayDeque *ad, void *out);
bool zzArrayDequeGet(const zzArrayDeque *ad, size_t idx, void *out);
void zzArrayDequeClear(zzArrayDeque *ad);

#endif
