#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#include "types.h"
#include "utils.h"
#include "result.h"

typedef struct DLNode {
    struct DLNode *prev;
    struct DLNode *next;
    unsigned char data[];
} DLNode;

typedef struct zzLinkedList {
    DLNode *head;
    DLNode *tail;
    size_t elSize;
    size_t size;
    zzFreeFn elemFree;
} zzLinkedList;

zzOpResult zzLinkedListInit(zzLinkedList *ll, size_t elSize, zzFreeFn elemFree);

void zzLinkedListFree(zzLinkedList *ll);

zzOpResult zzLinkedListPushFront(zzLinkedList *ll, const void *elem);

zzOpResult zzLinkedListPushBack(zzLinkedList *ll, const void *elem);

zzOpResult zzLinkedListPopFront(zzLinkedList *ll, void *out);

zzOpResult zzLinkedListPopBack(zzLinkedList *ll, void *out);

zzOpResult zzLinkedListGet(const zzLinkedList *ll, size_t idx, void *out);

zzOpResult zzLinkedListInsert(zzLinkedList *ll, size_t idx, const void *elem);

zzOpResult zzLinkedListRemove(zzLinkedList *ll, size_t idx);

void zzLinkedListClear(zzLinkedList *ll);

#endif