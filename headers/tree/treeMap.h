#ifndef TREE_MAP_H
#define TREE_MAP_H

#include "types.h"
#include "utils.h"
#include "result.h"

typedef struct TreeMapNode {
    struct TreeMapNode *left;
    struct TreeMapNode *right;
    struct TreeMapNode *parent;
    zzRBColor color;
    unsigned char data[];
} TreeMapNode;

typedef struct zzTreeMap {
    TreeMapNode *root;
    size_t size;
    size_t keySize;
    size_t valueSize;
    zzCompareFn compareFn;
    zzFreeFn keyFree;
    zzFreeFn valueFree;
} zzTreeMap;

zzOpResult zzTreeMapInit(zzTreeMap *tm, size_t keySize, size_t valueSize, zzCompareFn compareFn, zzFreeFn keyFree, zzFreeFn valueFree);

void zzTreeMapFree(zzTreeMap *tm);

zzOpResult zzTreeMapPut(zzTreeMap *tm, const void *key, const void *value);

zzOpResult zzTreeMapGet(const zzTreeMap *tm, const void *key, void *valueOut);

bool zzTreeMapContains(const zzTreeMap *tm, const void *key);

zzOpResult zzTreeMapRemove(zzTreeMap *tm, const void *key);

void zzTreeMapClear(zzTreeMap *tm);

zzOpResult zzTreeMapGetMin(const zzTreeMap *tm, void *keyOut, void *valueOut);

zzOpResult zzTreeMapGetMax(const zzTreeMap *tm, void *keyOut, void *valueOut);

#endif