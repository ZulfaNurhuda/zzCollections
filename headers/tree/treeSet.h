#ifndef TREE_SET_H
#define TREE_SET_H

#include "types.h"
#include "utils.h"
#include "result.h"

typedef enum { TS_RED, TS_BLACK } TSColor;

typedef struct TreeSetNode {
    struct TreeSetNode *left;
    struct TreeSetNode *right;
    struct TreeSetNode *parent;
    TSColor color;
    unsigned char key[];
} TreeSetNode;

typedef struct zzTreeSet {
    TreeSetNode *root;
    size_t keySize;
    size_t size;
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