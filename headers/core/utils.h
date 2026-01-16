#ifndef ZZ_UTILS_H
#define ZZ_UTILS_H

#include "types.h"

uint32_t zzDefaultHash(const void *key);

bool zzDefaultEquals(const void *a, const void *b);

int zzDefaultCompare(const void *a, const void *b);

uint32_t zzCharHash(const void *key);

bool zzCharEquals(const void *a, const void *b);

int zzCharCompare(const void *a, const void *b);

uint32_t zzIntHash(const void *key);

bool zzIntEquals(const void *a, const void *b);

int zzIntCompare(const void *a, const void *b);

uint32_t zzLongHash(const void *key);

bool zzLongEquals(const void *a, const void *b);

int zzLongCompare(const void *a, const void *b);

uint32_t zzFloatHash(const void *key);

bool zzFloatEquals(const void *a, const void *b);

int zzFloatCompare(const void *a, const void *b);

uint32_t zzDoubleHash(const void *key);

bool zzDoubleEquals(const void *a, const void *b);

int zzDoubleCompare(const void *a, const void *b);

uint32_t zzStringHash(const void *key);

bool zzStringEquals(const void *a, const void *b);

int zzStringCompare(const void *a, const void *b);

#endif