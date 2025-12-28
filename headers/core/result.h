#ifndef RESULT_H
#define RESULT_H

#include <stdlib.h>

/* =========================================================
 * Result Status
 * ========================================================= */
typedef enum ResultStatus {
    RESULT_SUCCESS = 0,
    RESULT_ERROR   = 1
} ResultStatus;

/* =========================================================
 * Result Type (Tagged Union)
 * ========================================================= */
typedef struct Result {
    ResultStatus status;
    union {
        void *data;        /* valid if status == RESULT_SUCCESS */
        const char *err;   /* valid if status == RESULT_ERROR   */
    } payload;
} Result;

/* =========================================================
 * SETTERS
 * ========================================================= */

#define RESULT_SET_OK(ptr) \
    ((Result){ .status = RESULT_SUCCESS, .payload.data = (ptr) })

#define RESULT_SET_ERR(msg) \
    ((Result){ .status = RESULT_ERROR, .payload.err = (msg) })

/* =========================================================
 * STATUS CHECK
 * ========================================================= */

#define RESULT_IS_OK(res) \
    ((res).status == RESULT_SUCCESS)

#define RESULT_IS_ERR(res) \
    ((res).status == RESULT_ERROR)

/* =========================================================
 * GETTERS
 * (Caller MUST check status first)
 * ========================================================= */

#define RESULT_GET_DATA(res) \
    ((res).payload.data)

#define RESULT_GET_ERR(res) \
    ((res).payload.err)

/* =========================================================
 * MEMORY MANAGEMENT
 * ========================================================= */

/*
 * Free ONLY the success data.
 * Does NOT change result status.
 *
 * WARNING:
 * - payload.data MUST be heap-allocated
 * - ownership must belong to caller
 */
#define RESULT_FREE_DATA(res)                      \
    do {                                           \
        if ((res).status == RESULT_SUCCESS &&      \
            (res).payload.data != NULL) {          \
            free((res).payload.data);              \
            (res).payload.data = NULL;             \
        }                                          \
    } while (0)

/* =========================================================
 * LIFECYCLE / STATE MANAGEMENT
 * ========================================================= */

/*
 * Reset result to error state.
 * Useful after freeing data or before reuse.
 */
#define RESULT_RESET(res)                          \
    do {                                           \
        (res).status = RESULT_ERROR;               \
        (res).payload.err = "result reset";        \
    } while (0)

/* =========================================================
 * OPERATION RESULT TYPE (For Collections Library)
 * ========================================================= */

/**
 * @brief Result type for operations that don't return data
 *
 * Used for Init, Add, Remove, and other operations in the collections
 * library that only need to indicate success or failure with an error message.
 */
typedef struct zzOpResult {
    ResultStatus status;   /**< SUCCESS or ERROR */
    const char *error;     /**< Error message if status == RESULT_ERROR, NULL otherwise */
} zzOpResult;

/* =========================================================
 * OPERATION RESULT MACROS
 * ========================================================= */

/**
 * @brief Create a successful operation result
 * @return zzOpResult with SUCCESS status
 */
#define ZZ_OK() \
    ((zzOpResult){ .status = RESULT_SUCCESS, .error = NULL })

/**
 * @brief Create an error operation result
 * @param msg Error message (must be string literal or static string)
 * @return zzOpResult with ERROR status and error message
 */
#define ZZ_ERR(msg) \
    ((zzOpResult){ .status = RESULT_ERROR, .error = (msg) })

/**
 * @brief Check if operation result is successful
 * @param res zzOpResult to check
 * @return true if status is SUCCESS, false otherwise
 */
#define ZZ_IS_OK(res) \
    ((res).status == RESULT_SUCCESS)

/**
 * @brief Check if operation result is an error
 * @param res zzOpResult to check
 * @return true if status is ERROR, false otherwise
 */
#define ZZ_IS_ERR(res) \
    ((res).status == RESULT_ERROR)

/**
 * @brief Get error message from operation result
 * @param res zzOpResult to get error from
 * @return Error message string (NULL if success)
 */
#define ZZ_GET_ERR(res) \
    ((res).error)

#endif /* RESULT_H */