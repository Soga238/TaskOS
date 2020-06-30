#ifndef __DLINK_LIST_H__
#define __DLINK_LIST_H__

#include ".\tk_types.h"

#define MALLOC(__SIZE)    malloc(__SIZE)
#define FREE(__PTR)       free(__PTR)

typedef struct _dlink_node {
    void *pData;
    struct _dlink_node *ptPre;
    struct _dlink_node *ptNext;
} dlink_node_t;

typedef struct _dlink_list {
    dlink_node_t *ptHead;
    int32_t nCount;
} dlink_list_t;

#define DLINK_LIST_INSERT(__LIST, __INDEX, __DATA)  \
    dlink_list_insert(__LIST, __INDEX, __DATA)

#define DLINK_LIST_GET(__LIST, __INDEX)             \
    dlink_list_get(__LIST, __INDEX)

#define DLINK_LIST_POP(__LIST, __INDEX)             \
    dlink_list_pop(__LIST, __INDEX)

#define DLINK_LIST_SIZE(__LIST)                     \
    dlink_list_size(__LIST)

#define DLINK_LIST_INSERT_FIRST(__LIST, __DATA)     \
    DLINK_LIST_INSERT(__LIST, 0, __DATA)

#define DLINK_LIST_GET_FIRST(__LIST)                \
    DLINK_LIST_GET(__LIST, 0)

#define DLINK_LIST_INSERT_LAST(__LIST, __DATA)      \
    dlink_list_insert(__LIST, DLINK_LIST_SIZE(__LIST), __DATA)

#define DLINK_LIST_GET_LAST(__LIST)                 \
    DLINK_LIST_GET(__LIST, DLINK_LIST_SIZE(__LIST))

#define DLINK_LIST_POP_FIRST(__LIST)                \
    DLINK_LIST_POP(__LIST, 0)

extern int32_t dlink_list_init(dlink_list_t *ptList);

extern int32_t dlink_list_insert(dlink_list_t *ptList, int32_t nIndex, void *pData);

extern void   *dlink_list_get(dlink_list_t *ptList, int32_t nIndex);

extern int32_t dlink_list_size(dlink_list_t *ptList);

extern int32_t dlink_list_free(dlink_list_t *ptList, int32_t nIndex);

extern void   *dlink_list_pop(dlink_list_t *ptList, int32_t nIndex);

#endif
