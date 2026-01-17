/**
 * @file result.h
 * @brief Result type definitions for error handling in the zzCollections library.
 *
 * This module provides a standardized way to handle operation results and errors
 * throughout the zzCollections library. It defines a result type that encapsulates
 * both success and error states, allowing functions to return detailed information
 * about their execution outcome.
 */

#ifndef ZZ_RESULT_H
#define ZZ_RESULT_H

/**
 * @brief Enumeration representing the possible statuses of an operation result.
 */
typedef enum {
    ZZ_SUCCESS = 0,  /**< Operation completed successfully */
    ZZ_ERROR   = 1   /**< Operation failed with an error */
} zzResultStatus;

/**
 * @brief Structure representing the result of an operation.
 *
 * This structure contains information about whether an operation succeeded or failed,
 * along with an optional error message in case of failure.
 */
typedef struct {
    zzResultStatus status;  /**< Status of the operation (success or error) */
    const char *error;      /**< Error message if status is ZZ_ERROR, NULL otherwise */
} zzOpResult;

/**
 * @brief Creates a success result with no error.
 *
 * This macro creates a zzOpResult indicating that an operation completed successfully.
 * The status will be ZZ_SUCCESS and the error field will be NULL.
 *
 * @return A zzOpResult with ZZ_SUCCESS status and NULL error message
 */
#define ZZ_OK() \
    ((zzOpResult){ .status = ZZ_SUCCESS, .error = NULL })

/**
 * @brief Creates an error result with the specified error message.
 *
 * This macro creates a zzOpResult indicating that an operation failed with the
 * specified error message. The status will be ZZ_ERROR and the error field will
 * contain the provided message.
 *
 * @param msg The error message string to associate with the error result
 * @return A zzOpResult with ZZ_ERROR status and the provided error message
 */
#define ZZ_ERR(msg) \
    ((zzOpResult){ .status = ZZ_ERROR, .error = (msg) })

/**
 * @brief Checks if a result indicates success.
 *
 * This macro evaluates whether the given result represents a successful operation.
 *
 * @param res The zzOpResult to check
 * @return true if the result indicates success, false otherwise
 */
#define ZZ_IS_OK(res) \
    ((res).status == ZZ_SUCCESS)

/**
 * @brief Checks if a result indicates an error.
 *
 * This macro evaluates whether the given result represents a failed operation.
 *
 * @param res The zzOpResult to check
 * @return true if the result indicates an error, false otherwise
 */
#define ZZ_IS_ERR(res) \
    ((res).status == ZZ_ERROR)

/**
 * @brief Extracts the error message from a result.
 *
 * This macro retrieves the error message from a result. If the result indicates
 * success, this will return NULL. If the result indicates an error, this will
 * return the associated error message.
 *
 * @param res The zzOpResult from which to extract the error message
 * @return The error message string, or NULL if the result indicates success
 */
#define ZZ_GET_ERR(res) \
    ((res).error)

#endif