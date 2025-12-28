#include "arrayList.h"
#include "memory.h"
#include <stdlib.h>

zzOpResult zzArrayListInit(zzArrayList *al, size_t elSize, size_t capacity, zzFreeFn elemFree) {
    if (!al) return ZZ_ERR("ArrayList pointer is NULL");
    if (elSize == 0) return ZZ_ERR("Element size cannot be zero");
    if (capacity == 0) capacity = 4;

    al->elSize = elSize;
    al->size = 0;
    al->capacity = capacity;
    al->elemFree = elemFree;
    al->buffer = malloc(elSize * capacity);

    if (!al->buffer) return ZZ_ERR("Failed to allocate buffer memory");
    return ZZ_OK();
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

static zzOpResult zzArrayListGrow(zzArrayList *al) {
    size_t newCap = al->capacity * 2;
    void *newBuf = realloc(al->buffer, al->elSize * newCap);
    if (!newBuf) return ZZ_ERR("Failed to grow buffer (realloc failed)");
    al->buffer = newBuf;
    al->capacity = newCap;
    return ZZ_OK();
}

zzOpResult zzArrayListAdd(zzArrayList *al, const void *elem) {
    if (!al) return ZZ_ERR("ArrayList pointer is NULL");
    if (!elem) return ZZ_ERR("Element pointer is NULL");

    if (al->size == al->capacity) {
        zzOpResult growResult = zzArrayListGrow(al);
        if (ZZ_IS_ERR(growResult)) {
            return growResult;  // Propagate error
        }
    }

    zzMemoryCopy((char*)al->buffer + al->size * al->elSize, elem, al->elSize);
    al->size++;
    return ZZ_OK();
}

zzOpResult zzArrayListGet(const zzArrayList *al, size_t idx, void *out) {
    if (!al) return ZZ_ERR("ArrayList pointer is NULL");
    if (!out) return ZZ_ERR("Output buffer is NULL");
    if (idx >= al->size) return ZZ_ERR("Index out of bounds");

    zzMemoryCopy(out, (char*)al->buffer + idx * al->elSize, al->elSize);
    return ZZ_OK();
}

zzOpResult zzArrayListSet(zzArrayList *al, size_t idx, const void *elem) {
    if (!al) return ZZ_ERR("ArrayList pointer is NULL");
    if (!elem) return ZZ_ERR("Element pointer is NULL");
    if (idx >= al->size) return ZZ_ERR("Index out of bounds");

    void *target = (char*)al->buffer + idx * al->elSize;
    if (al->elemFree) {
        al->elemFree(target);
    }
    zzMemoryCopy(target, elem, al->elSize);
    return ZZ_OK();
}

zzOpResult zzArrayListRemove(zzArrayList *al, size_t idx) {
    if (!al) return ZZ_ERR("ArrayList pointer is NULL");
    if (idx >= al->size) return ZZ_ERR("Index out of bounds");

    void *target = (char*)al->buffer + idx * al->elSize;
    if (al->elemFree) {
        al->elemFree(target);
    }

    // Shift elements left
    if (idx < al->size - 1) {
        zzMemoryCopy(target, (char*)target + al->elSize,
                     (al->size - idx - 1) * al->elSize);
    }
    al->size--;
    return ZZ_OK();
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

zzOpResult zzArrayListInsert(zzArrayList *al, size_t idx, const void *elem) {
    if (!al) return ZZ_ERR("ArrayList pointer is NULL");
    if (!elem) return ZZ_ERR("Element pointer is NULL");
    if (idx > al->size) return ZZ_ERR("Index out of bounds");

    if (al->size == al->capacity) {
        zzOpResult growResult = zzArrayListGrow(al);
        if (ZZ_IS_ERR(growResult)) {
            return growResult;
        }
    }

    // Shift elements right
    if (idx < al->size) {
        void *src = (char*)al->buffer + idx * al->elSize;
        void *dst = (char*)src + al->elSize;
        zzMemoryCopy(dst, src, (al->size - idx) * al->elSize);
    }

    zzMemoryCopy((char*)al->buffer + idx * al->elSize, elem, al->elSize);
    al->size++;
    return ZZ_OK();
}

zzOpResult zzArrayListIndexOf(const zzArrayList *al, const void *elem, zzCompareFn cmp, int *indexOut) {
    if (!al) return ZZ_ERR("ArrayList pointer is NULL");
    if (!elem) return ZZ_ERR("Element pointer is NULL");
    if (!cmp) return ZZ_ERR("Comparison function is NULL");
    if (!indexOut) return ZZ_ERR("Index output pointer is NULL");

    for (size_t i = 0; i < al->size; i++) {
        void *item = (char*)al->buffer + i * al->elSize;
        if (cmp(item, elem) == 0) {
            *indexOut = (int)i;
            return ZZ_OK();
        }
    }
    return ZZ_ERR("Element not found");
}
