/**
 * @file iterator.h
 * @brief Common iterator types and definitions for zzCollections library.
 *
 * This module defines common iterator types and enumerations used across
 * various data structures in the zzCollections library. These types provide
 * a consistent interface for iterating through different collection types.
 */

#ifndef ZZ_ITERATOR_H
#define ZZ_ITERATOR_H

#include <stdbool.h>

/**
 * @brief Enumeration representing iterator states.
 *
 * This enumeration is used to track the current state of an iterator,
 * indicating whether it's valid, has reached the end, or encountered an error.
 */
typedef enum {
    ZZ_ITER_VALID,    /**< Iterator is valid and pointing to an element */
    ZZ_ITER_END,      /**< Iterator has reached the end of the collection */
    ZZ_ITER_ERROR     /**< Iterator encountered an error */
} zzIteratorState;

#endif
