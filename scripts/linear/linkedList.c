#include "linkedList.h"
#include "memory.h"
#include <stdlib.h>

bool zzLinkedListInit(zzLinkedList *ll, size_t elSize, zzFreeFn elemFree) {
    if (!ll || elSize == 0) return false;
    
    ll->head = NULL;
    ll->tail = NULL;
    ll->elSize = elSize;
    ll->size = 0;
    ll->elemFree = elemFree;
    return true;
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

bool zzLinkedListPushFront(zzLinkedList *ll, const void *elem) {
    if (!ll || !elem) return false;
    
    DLNode *n = malloc(sizeof(DLNode) + ll->elSize);
    if (!n) return false;
    
    zzMemoryCopy(n->data, elem, ll->elSize);
    n->prev = NULL;
    n->next = ll->head;
    
    if (ll->head) ll->head->prev = n;
    ll->head = n;
    if (!ll->tail) ll->tail = n;
    
    ll->size++;
    return true;
}

bool zzLinkedListPushBack(zzLinkedList *ll, const void *elem) {
    if (!ll || !elem) return false;
    
    DLNode *n = malloc(sizeof(DLNode) + ll->elSize);
    if (!n) return false;
    
    zzMemoryCopy(n->data, elem, ll->elSize);
    n->prev = ll->tail;
    n->next = NULL;
    
    if (ll->tail) ll->tail->next = n;
    ll->tail = n;
    if (!ll->head) ll->head = n;
    
    ll->size++;
    return true;
}

bool zzLinkedListPopFront(zzLinkedList *ll, void *out) {
    if (!ll || !ll->head) return false;
    
    DLNode *tmp = ll->head;
    if (out) zzMemoryCopy(out, tmp->data, ll->elSize);
    
    ll->head = ll->head->next;
    if (ll->head) ll->head->prev = NULL;
    else ll->tail = NULL;
    
    if (ll->elemFree && !out) ll->elemFree(tmp->data);
    free(tmp);
    ll->size--;
    return true;
}

bool zzLinkedListPopBack(zzLinkedList *ll, void *out) {
    if (!ll || !ll->tail) return false;
    
    DLNode *tmp = ll->tail;
    if (out) zzMemoryCopy(out, tmp->data, ll->elSize);
    
    ll->tail = ll->tail->prev;
    if (ll->tail) ll->tail->next = NULL;
    else ll->head = NULL;
    
    if (ll->elemFree && !out) ll->elemFree(tmp->data);
    free(tmp);
    ll->size--;
    return true;
}

bool zzLinkedListGet(const zzLinkedList *ll, size_t idx, void *out) {
    if (!ll || !out || idx >= ll->size) return false;
    
    DLNode *cur;
    if (idx < ll->size / 2) {
        cur = ll->head;
        for (size_t i = 0; i < idx; i++) cur = cur->next;
    } else {
        cur = ll->tail;
        for (size_t i = ll->size - 1; i > idx; i--) cur = cur->prev;
    }
    
    zzMemoryCopy(out, cur->data, ll->elSize);
    return true;
}

bool zzLinkedListInsert(zzLinkedList *ll, size_t idx, const void *elem) {
    if (!ll || !elem || idx > ll->size) return false;
    if (idx == 0) return zzLinkedListPushFront(ll, elem);
    if (idx == ll->size) return zzLinkedListPushBack(ll, elem);
    
    DLNode *n = malloc(sizeof(DLNode) + ll->elSize);
    if (!n) return false;
    zzMemoryCopy(n->data, elem, ll->elSize);
    
    DLNode *cur = ll->head;
    for (size_t i = 0; i < idx; i++) cur = cur->next;
    
    n->next = cur;
    n->prev = cur->prev;
    cur->prev->next = n;
    cur->prev = n;
    
    ll->size++;
    return true;
}

bool zzLinkedListRemove(zzLinkedList *ll, size_t idx) {
    if (!ll || idx >= ll->size) return false;
    if (idx == 0) return zzLinkedListPopFront(ll, NULL);
    if (idx == ll->size - 1) return zzLinkedListPopBack(ll, NULL);
    
    DLNode *cur = ll->head;
    for (size_t i = 0; i < idx; i++) cur = cur->next;
    
    cur->prev->next = cur->next;
    cur->next->prev = cur->prev;
    
    if (ll->elemFree) ll->elemFree(cur->data);
    free(cur);
    ll->size--;
    return true;
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
