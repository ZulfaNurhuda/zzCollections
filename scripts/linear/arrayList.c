#include "arrayList.h"
#include "memory.h"
#include <stdlib.h>

bool zzArrayListInit(zzArrayList *al, size_t elSize, size_t capacity, zzFreeFn elemFree) {
    if (!al || elSize == 0) return false;
    if (capacity == 0) capacity = 4;
    
    al->elSize = elSize;
    al->size = 0;
    al->capacity = capacity;
    al->elemFree = elemFree;
    al->buffer = malloc(elSize * capacity);
    return al->buffer != NULL;
}

void zzArrayListFree(zzArrayList *al) {
    if (!al || !al->buffer) return;
    
    if (al->elemFree) {
        for (size_t i = 0; i < al->size; i++) {
            void *elem = (char*)al->buffer + i * al->elSize;
            al->elemFree(elem);
        }
    }
    free(al->buffer);
    al->buffer = NULL;
    al->size = 0;
}

static bool zzArrayListGrow(zzArrayList *al) {
    size_t newCap = al->capacity * 2;
    void *newBuf = realloc(al->buffer, al->elSize * newCap);
    if (!newBuf) return false;
    al->buffer = newBuf;
    al->capacity = newCap;
    return true;
}

bool zzArrayListAdd(zzArrayList *al, const void *elem) {
    if (!al || !elem) return false;
    if (al->size == al->capacity && !zzArrayListGrow(al)) return false;
    
    zzMemoryCopy((char*)al->buffer + al->size * al->elSize, elem, al->elSize);
    al->size++;
    return true;
}

bool zzArrayListGet(const zzArrayList *al, size_t idx, void *out) {
    if (!al || !out || idx >= al->size) return false;
    zzMemoryCopy(out, (char*)al->buffer + idx * al->elSize, al->elSize);
    return true;
}

bool zzArrayListSet(zzArrayList *al, size_t idx, const void *elem) {
    if (!al || !elem || idx >= al->size) return false;
    
    void *target = (char*)al->buffer + idx * al->elSize;
    if (al->elemFree) al->elemFree(target);
    zzMemoryCopy(target, elem, al->elSize);
    return true;
}

bool zzArrayListRemove(zzArrayList *al, size_t idx) {
    if (!al || idx >= al->size) return false;
    
    void *target = (char*)al->buffer + idx * al->elSize;
    if (al->elemFree) al->elemFree(target);
    
    if (idx < al->size - 1) {
        zzMemoryCopy(target, (char*)target + al->elSize,
                   (al->size - idx - 1) * al->elSize);
    }
    al->size--;
    return true;
}

void zzArrayListClear(zzArrayList *al) {
    if (!al) return;
    
    if (al->elemFree) {
        for (size_t i = 0; i < al->size; i++) {
            void *elem = (char*)al->buffer + i * al->elSize;
            al->elemFree(elem);
        }
    }
    al->size = 0;
}

bool zzArrayListInsert(zzArrayList *al, size_t idx, const void *elem) {
    if (!al || !elem || idx > al->size) return false;
    if (al->size == al->capacity && !zzArrayListGrow(al)) return false;
    
    if (idx < al->size) {
        void *src = (char*)al->buffer + idx * al->elSize;
        void *dst = (char*)src + al->elSize;
        zzMemoryCopy(dst, src, (al->size - idx) * al->elSize);
    }
    
    zzMemoryCopy((char*)al->buffer + idx * al->elSize, elem, al->elSize);
    al->size++;
    return true;
}

int zzArrayListIndexOf(const zzArrayList *al, const void *elem, zzCompareFn cmp) {
    if (!al || !elem || !cmp) return -1;
    
    for (size_t i = 0; i < al->size; i++) {
        void *item = (char*)al->buffer + i * al->elSize;
        if (cmp(item, elem) == 0) return (int)i;
    }
    return -1;
}
