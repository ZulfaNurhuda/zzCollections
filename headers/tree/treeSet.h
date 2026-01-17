#ifndef TREE_SET_H
#define TREE_SET_H

#include "types.h"
#include "utils.h"
#include "result.h"

typedef struct TreeSetNode {
    struct TreeSetNode *left;
    struct TreeSetNode *right;
    struct TreeSetNode *parent;
    zzRBColor color;
    unsigned char key[];
} TreeSetNode;

typedef struct zzTreeSet {
    TreeSetNode *root;
    size_t size;
    size_t keySize;
    zzCompareFn compareFn;
    zzFreeFn keyFree;
} zzTreeSet;

zzOpResult zzTreeSetInit(zzTreeSet *ts, size_t keySize, zzCompareFn compareFn, zzFreeFn keyFree);

void zzTreeSetFree(zzTreeSet *ts);

zzOpResult zzTreeSetInsert(zzTreeSet *ts, const void *key);

bool zzTreeSetContains(const zzTreeSet *ts, const void *key);

zzOpResult zzTreeSetRemove(zzTreeSet *ts, const void *key);

void zzTreeSetClear(zzTreeSet *ts);

zzOpResult zzTreeSetGetMin(const zzTreeSet *ts, void *keyOut);

zzOpResult zzTreeSetGetMax(const zzTreeSet *ts, void *keyOut);

#endif