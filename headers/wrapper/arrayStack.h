#ifndef ARRAY_STACK_H
#define ARRAY_STACK_H

#include "arrayDeque.h"

typedef zzArrayDeque zzArrayStack;

static inline zzOpResult zzArrayStackInit(zzArrayStack *s, size_t elSize, size_t capacity, zzFreeFn elemFree) {
    return zzArrayDequeInit(s, elSize, capacity, elemFree);
}

static inline void zzArrayStackFree(zzArrayStack *s) {
    zzArrayDequeFree(s);
}

static inline zzOpResult zzArrayStackPush(zzArrayStack *s, const void *elem) {
    return zzArrayDequePushBack(s, elem);
}

static inline zzOpResult zzArrayStackPop(zzArrayStack *s, void *out) {
    return zzArrayDequePopBack(s, out);
}

static inline zzOpResult zzArrayStackPeek(const zzArrayStack *s, void *out) {
    return zzArrayDequePeekBack(s, out);
}

static inline bool zzArrayStackIsEmpty(const zzArrayStack *s) {
    return s->size == 0;
}

static inline size_t zzArrayStackSize(const zzArrayStack *s) {
    return s->size;
}

static inline void zzArrayStackClear(zzArrayStack *s) {
    zzArrayDequeClear(s);
}

#endif