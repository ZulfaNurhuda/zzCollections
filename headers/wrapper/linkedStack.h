#ifndef LINKED_STACK_H
#define LINKED_STACK_H

#include "linkedList.h"
#include <string.h>

typedef zzLinkedList zzLinkedStack;

static inline zzOpResult zzLinkedStackInit(zzLinkedStack *s, size_t elSize, zzFreeFn elemFree) {
    return zzLinkedListInit(s, elSize, elemFree);
}

static inline void zzLinkedStackFree(zzLinkedStack *s) {
    zzLinkedListFree(s);
}

static inline zzOpResult zzLinkedStackPush(zzLinkedStack *s, const void *elem) {
    return zzLinkedListPushBack(s, elem);
}

static inline zzOpResult zzLinkedStackPop(zzLinkedStack *s, void *out) {
    return zzLinkedListPopBack(s, out);
}

static inline zzOpResult zzLinkedStackPeek(const zzLinkedStack *s, void *out) {
    if (!s) return ZZ_ERR("LinkedList pointer is NULL");
    if (!out) return ZZ_ERR("Output buffer is NULL");
    if (!s->tail) return ZZ_ERR("List is empty");

    memcpy(out, s->tail->data, s->elSize);
    return ZZ_OK();
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