#include "linkedList.h"
#include <string.h>
#include <stdlib.h>

zzOpResult zzLinkedListInit(zzLinkedList *ll, size_t elSize, zzFreeFn elemFree) {
    if (!ll) return ZZ_ERR("LinkedList pointer is NULL");
    if (elSize == 0) return ZZ_ERR("Element size cannot be zero");

    ll->head = NULL;
    ll->tail = NULL;
    ll->elSize = elSize;
    ll->size = 0;
    ll->elemFree = elemFree;
    return ZZ_OK();
}

void zzLinkedListFree(zzLinkedList *ll) {
    if (!ll) return;

    DLNode *cur = ll->head;
    while (cur) {
        DLNode *next = cur->next;
        if (ll->elemFree) ll->elemFree(cur->data);
        free(cur);
        cur = next;
    }
    ll->head = ll->tail = NULL;
    ll->size = 0;
}

zzOpResult zzLinkedListPushFront(zzLinkedList *ll, const void *elem) {
    if (!ll) return ZZ_ERR("LinkedList pointer is NULL");
    if (!elem) return ZZ_ERR("Element pointer is NULL");

    DLNode *n = malloc(sizeof(DLNode) + ll->elSize);
    if (!n) return ZZ_ERR("Failed to allocate node");

    memcpy(n->data, elem, ll->elSize);
    n->prev = NULL;
    n->next = ll->head;

    if (ll->head) ll->head->prev = n;
    ll->head = n;
    if (!ll->tail) ll->tail = n;

    ll->size++;
    return ZZ_OK();
}

zzOpResult zzLinkedListPushBack(zzLinkedList *ll, const void *elem) {
    if (!ll) return ZZ_ERR("LinkedList pointer is NULL");
    if (!elem) return ZZ_ERR("Element pointer is NULL");

    DLNode *n = malloc(sizeof(DLNode) + ll->elSize);
    if (!n) return ZZ_ERR("Failed to allocate node");

    memcpy(n->data, elem, ll->elSize);
    n->prev = ll->tail;
    n->next = NULL;

    if (ll->tail) ll->tail->next = n;
    ll->tail = n;
    if (!ll->head) ll->head = n;

    ll->size++;
    return ZZ_OK();
}

zzOpResult zzLinkedListPopFront(zzLinkedList *ll, void *out) {
    if (!ll) return ZZ_ERR("LinkedList pointer is NULL");
    if (!out) return ZZ_ERR("Output buffer is NULL");
    if (!ll->head) return ZZ_ERR("List is empty");

    DLNode *tmp = ll->head;
    memcpy(out, tmp->data, ll->elSize);

    ll->head = ll->head->next;
    if (ll->head) ll->head->prev = NULL;
    else ll->tail = NULL;

    free(tmp);
    ll->size--;
    return ZZ_OK();
}

zzOpResult zzLinkedListPopBack(zzLinkedList *ll, void *out) {
    if (!ll) return ZZ_ERR("LinkedList pointer is NULL");
    if (!out) return ZZ_ERR("Output buffer is NULL");
    if (!ll->tail) return ZZ_ERR("List is empty");

    DLNode *tmp = ll->tail;
    memcpy(out, tmp->data, ll->elSize);

    ll->tail = ll->tail->prev;
    if (ll->tail) ll->tail->next = NULL;
    else ll->head = NULL;

    free(tmp);
    ll->size--;
    return ZZ_OK();
}

zzOpResult zzLinkedListGet(const zzLinkedList *ll, size_t idx, void *out) {
    if (!ll) return ZZ_ERR("LinkedList pointer is NULL");
    if (!out) return ZZ_ERR("Output buffer is NULL");
    if (idx >= ll->size) return ZZ_ERR("Index out of bounds");

    DLNode *cur;
    if (idx < ll->size / 2) {
        cur = ll->head;
        for (size_t i = 0; i < idx; i++) cur = cur->next;
    } else {
        cur = ll->tail;
        for (size_t i = ll->size - 1; i > idx; i--) cur = cur->prev;
    }

    memcpy(out, cur->data, ll->elSize);
    return ZZ_OK();
}

zzOpResult zzLinkedListInsert(zzLinkedList *ll, size_t idx, const void *elem) {
    if (!ll) return ZZ_ERR("LinkedList pointer is NULL");
    if (!elem) return ZZ_ERR("Element pointer is NULL");
    if (idx > ll->size) return ZZ_ERR("Index out of bounds");
    if (idx == 0) return zzLinkedListPushFront(ll, elem);
    if (idx == ll->size) return zzLinkedListPushBack(ll, elem);

    DLNode *n = malloc(sizeof(DLNode) + ll->elSize);
    if (!n) return ZZ_ERR("Failed to allocate node");
    memcpy(n->data, elem, ll->elSize);

    DLNode *cur = ll->head;
    for (size_t i = 0; i < idx; i++) cur = cur->next;

    n->next = cur;
    n->prev = cur->prev;
    cur->prev->next = n;
    cur->prev = n;

    ll->size++;
    return ZZ_OK();
}

zzOpResult zzLinkedListRemove(zzLinkedList *ll, size_t idx) {
    if (!ll) return ZZ_ERR("LinkedList pointer is NULL");
    if (idx >= ll->size) return ZZ_ERR("Index out of bounds");

    if (idx == 0) {
        if (!ll->head) return ZZ_ERR("List is empty");
        DLNode *tmp = ll->head;
        ll->head = ll->head->next;
        if (ll->head) ll->head->prev = NULL;
        else ll->tail = NULL;
        if (ll->elemFree) ll->elemFree(tmp->data);
        free(tmp);
        ll->size--;
        return ZZ_OK();
    }

    if (idx == ll->size - 1) {
        if (!ll->tail) return ZZ_ERR("List is empty");
        DLNode *tmp = ll->tail;
        ll->tail = ll->tail->prev;
        if (ll->tail) ll->tail->next = NULL;
        else ll->head = NULL;
        if (ll->elemFree) ll->elemFree(tmp->data);
        free(tmp);
        ll->size--;
        return ZZ_OK();
    }

    DLNode *cur = ll->head;
    for (size_t i = 0; i < idx; i++) cur = cur->next;

    cur->prev->next = cur->next;
    cur->next->prev = cur->prev;

    if (ll->elemFree) ll->elemFree(cur->data);
    free(cur);
    ll->size--;
    return ZZ_OK();
}

void zzLinkedListClear(zzLinkedList *ll) {
    if (!ll) return;

    DLNode *cur = ll->head;
    while (cur) {
        DLNode *next = cur->next;
        if (ll->elemFree) ll->elemFree(cur->data);
        free(cur);
        cur = next;
    }
    ll->head = ll->tail = NULL;
    ll->size = 0;
}