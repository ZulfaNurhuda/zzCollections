#ifndef MEMORY_H
#define MEMORY_H

#include <stddef.h>
#include <stdint.h>

/**
 * Optimized memory copy function that handles both forward and backward copying.
 * Can replace both memcpy and memmove.
 * 
 * @param dest Destination pointer
 * @param src Source pointer
 * @param n Number of bytes to copy
 * @return Pointer to destination
 */
void *zzMemoryCopy(void *dest, const void *src, size_t n);

#endif // MEMORY_H
