#ifndef ZZ_RESULT_H
#define ZZ_RESULT_H

typedef enum {
    ZZ_SUCCESS = 0,
    ZZ_ERROR   = 1
} zzResultStatus;

typedef struct {
    zzResultStatus status;
    const char *error;
} zzOpResult;

#define ZZ_OK() \
    ((zzOpResult){ .status = ZZ_SUCCESS, .error = NULL })

#define ZZ_ERR(msg) \
    ((zzOpResult){ .status = ZZ_ERROR, .error = (msg) })

#define ZZ_IS_OK(res) \
    ((res).status == ZZ_SUCCESS)

#define ZZ_IS_ERR(res) \
    ((res).status == ZZ_ERROR)

#define ZZ_GET_ERR(res) \
    ((res).error)

#endif