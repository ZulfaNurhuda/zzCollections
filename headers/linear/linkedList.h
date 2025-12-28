#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#include "common.h"

typedef struct DLNode {
    struct DLNode *prev;
    struct DLNode *next;
    unsigned char data[];
} DLNode;

typedef struct {
    DLNode *head;
    DLNode *tail;
    size_t elSize;
    size_t size;
    zzFreeFn elemFree;
} zzLinkedList;

bool zzLinkedListInit(zzLinkedList *ll, size_t elSize, zzFreeFn elemFree);
void zzLinkedListFree(zzLinkedList *ll);
bool zzLinkedListPushFront(zzLinkedList *ll, const void *elem);
bool zzLinkedListPushBack(zzLinkedList *ll, const void *elem);
bool zzLinkedListPopFront(zzLinkedList *ll, void *out);
bool zzLinkedListPopBack(zzLinkedList *ll, void *out);
bool zzLinkedListGet(const zzLinkedList *ll, size_t idx, void *out);
bool zzLinkedListInsert(zzLinkedList *ll, size_t idx, const void *elem);
bool zzLinkedListRemove(zzLinkedList *ll, size_t idx);
void zzLinkedListClear(zzLinkedList *ll);

#endif
