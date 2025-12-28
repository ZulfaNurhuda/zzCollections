#ifndef LINKED_STACK_H
#define LINKED_STACK_H

#include "linkedList.h"
#include "memory.h"

typedef zzLinkedList zzLinkedStack;

static inline bool zzLinkedStackInit(zzLinkedStack *s, size_t elSize, zzFreeFn elemFree) {
    return zzLinkedListInit(s, elSize, elemFree);
}

static inline void zzLinkedStackFree(zzLinkedStack *s) {
    zzLinkedListFree(s);
}

static inline bool zzLinkedStackPush(zzLinkedStack *s, const void *elem) {
    return zzLinkedListPushBack(s, elem);
}

static inline bool zzLinkedStackPop(zzLinkedStack *s, void *out) {
    return zzLinkedListPopBack(s, out);
}

static inline bool zzLinkedStackPeek(const zzLinkedStack *s, void *out) {
    if (!s || !out || !s->tail) return false;
    zzMemoryCopy(out, s->tail->data, s->elSize);
    return true;
}

static inline bool zzLinkedStackIsEmpty(const zzLinkedStack *s) {
    return s->size == 0;
}

static inline size_t zzLinkedStackSize(const zzLinkedStack *s) {
    return s->size;
}

static inline void zzLinkedStackClear(zzLinkedStack *s) {
    zzLinkedListClear(s);
}

#endif
