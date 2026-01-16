#ifndef ARRAY_LIST_H
#define ARRAY_LIST_H

#include "types.h"
#include "utils.h"
#include "result.h"

typedef struct zzArrayList {
    void *buffer;
    size_t elSize;
    size_t size;
    size_t capacity;
    zzFreeFn elemFree;
} zzArrayList;

zzOpResult zzArrayListInit(zzArrayList *al, size_t elSize, size_t capacity, zzFreeFn elemFree);

void zzArrayListFree(zzArrayList *al);

zzOpResult zzArrayListAdd(zzArrayList *al, const void *elem);

zzOpResult zzArrayListGet(const zzArrayList *al, size_t idx, void *out);

zzOpResult zzArrayListSet(zzArrayList *al, size_t idx, const void *elem);

zzOpResult zzArrayListRemove(zzArrayList *al, size_t idx);

void zzArrayListClear(zzArrayList *al);

zzOpResult zzArrayListInsert(zzArrayList *al, size_t idx, const void *elem);

zzOpResult zzArrayListIndexOf(const zzArrayList *al, const void *elem, zzCompareFn cmp, int *indexOut);

#endif