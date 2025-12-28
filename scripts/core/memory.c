#include "memory.h"

void *zzMemoryCopy(void *dest, const void *src, size_t n)
{
    if (dest == src || n == 0)
        return dest;

    uint8_t *d = (uint8_t *)dest;
    const uint8_t *s = (const uint8_t *)src;

    /* =====================================================
       COPY MAJU (dest < src)
       ===================================================== */
    if (d < s) {
        /* Byte sampai alignment 8 */
        while (n && (((uintptr_t)d | (uintptr_t)s) & 7)) {
            *d++ = *s++;
            n--;
        }

        /* 8 byte */
        while (n >= 8) {
            *(uint64_t *)d = *(const uint64_t *)s;
            d += 8;
            s += 8;
            n -= 8;
        }

        /* Byte sampai alignment 4 */
        while (n && (((uintptr_t)d | (uintptr_t)s) & 3)) {
            *d++ = *s++;
            n--;
        }

        /* 4 byte */
        while (n >= 4) {
            *(uint32_t *)d = *(const uint32_t *)s;
            d += 4;
            s += 4;
            n -= 4;
        }

        /* Sisa byte */
        while (n--) {
            *d++ = *s++;
        }
    }
    /* =====================================================
       COPY MUNDUR (dest > src)
       ===================================================== */
    else {
        d += n;
        s += n;

        /* Byte sampai alignment 8 (dari belakang) */
        while (n && (((uintptr_t)d | (uintptr_t)s) & 7)) {
            *--d = *--s;
            n--;
        }

        /* 8 byte */
        while (n >= 8) {
            d -= 8;
            s -= 8;
            *(uint64_t *)d = *(const uint64_t *)s;
            n -= 8;
        }

        /* Byte sampai alignment 4 */
        while (n && (((uintptr_t)d | (uintptr_t)s) & 3)) {
            *--d = *--s;
            n--;
        }

        /* 4 byte */
        while (n >= 4) {
            d -= 4;
            s -= 4;
            *(uint32_t *)d = *(const uint32_t *)s;
            n -= 4;
        }

        /* Sisa byte */
        while (n--) {
            *--d = *--s;
        }
    }

    return dest;
}
