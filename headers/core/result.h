/**
 * @file result.h
 * @brief Simple result type for operation status and error handling
 *
 * Provides a lightweight result type for functions that need to indicate
 * success or failure with an optional error message. Used throughout the
 * zzCollections library for consistent error handling.
 */

#ifndef ZZ_RESULT_H
#define ZZ_RESULT_H

/* =========================================================
 * Result Status Enumeration
 * ========================================================= */

/**
 * @brief Status codes for operation results
 */
typedef enum {
    ZZ_SUCCESS = 0,  /**< Operation completed successfully */
    ZZ_ERROR   = 1   /**< Operation failed with error */
} zzResultStatus;

/* =========================================================
 * Operation Result Type
 * ========================================================= */

/**
 * @brief Result type for operations in the collections library
 *
 * Used for Init, Add, Remove, and other operations that only need to
 * indicate success or failure with an error message.
 */
typedef struct {
    zzResultStatus status;  /**< Operation status (SUCCESS or ERROR) */
    const char *error;      /**< Error message if status == ERROR, NULL otherwise */
} zzOpResult;

/* =========================================================
 * Result Creation Macros
 * ========================================================= */

/**
 * @brief Create a successful operation result
 * @return zzOpResult with SUCCESS status and NULL error
 */
#define ZZ_OK() \
    ((zzOpResult){ .status = ZZ_SUCCESS, .error = NULL })

/**
 * @brief Create an error operation result
 * @param msg Error message (must be string literal or static string)
 * @return zzOpResult with ERROR status and error message
 */
#define ZZ_ERR(msg) \
    ((zzOpResult){ .status = ZZ_ERROR, .error = (msg) })

/* =========================================================
 * Status Check Macros
 * ========================================================= */

/**
 * @brief Check if operation result is successful
 * @param res zzOpResult to check
 * @return true if status is SUCCESS, false otherwise
 */
#define ZZ_IS_OK(res) \
    ((res).status == ZZ_SUCCESS)

/**
 * @brief Check if operation result is an error
 * @param res zzOpResult to check
 * @return true if status is ERROR, false otherwise
 */
#define ZZ_IS_ERR(res) \
    ((res).status == ZZ_ERROR)

/* =========================================================
 * Error Message Getter
 * ========================================================= */

/**
 * @brief Get error message from operation result
 * @param res zzOpResult to get error from
 * @return Error message string (NULL if success)
 */
#define ZZ_GET_ERR(res) \
    ((res).error)

#endif /* ZZ_RESULT_H */
