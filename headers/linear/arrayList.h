#ifndef ARRAY_LIST_H
#define ARRAY_LIST_H

#include "common.h"

typedef struct {
    void *buffer;
    size_t elSize;
    size_t size;
    size_t capacity;
    zzFreeFn elemFree;
} zzArrayList;

bool zzArrayListInit(zzArrayList *al, size_t elSize, size_t capacity, zzFreeFn elemFree);
void zzArrayListFree(zzArrayList *al);
bool zzArrayListAdd(zzArrayList *al, const void *elem);
bool zzArrayListGet(const zzArrayList *al, size_t idx, void *out);
bool zzArrayListSet(zzArrayList *al, size_t idx, const void *elem);
bool zzArrayListRemove(zzArrayList *al, size_t idx);
void zzArrayListClear(zzArrayList *al);
bool zzArrayListInsert(zzArrayList *al, size_t idx, const void *elem);
int zzArrayListIndexOf(const zzArrayList *al, const void *elem, zzCompareFn cmp);

#endif
