#ifndef __SLIDING_WINDOW_PRIV_H__
#define __SLIDING_WINDOW_PRIV_H__

#include "basic_types.h"

typedef struct
{
    USHORT head;
    USHORT tail;
    USHORT length;
    USHORT size;
    UINT32 values[];
}RING;

typedef struct
{
    SLIDING_WINDOW_THRESHOLD threshold;
    RING                     hitTicks;
}SLIDING_WINDOW;

#define SLIDING_WINDOW_SIZE(slidingNum) (sizeof(SLIDING_WINDOW) + sizeof(UINT32) * (slidingNum))
#define SLIDING_RING_SIZE(slidingNum) (sizeof(RING) + sizeof(UINT32) * (slidingNum))
#define RETURN_ERR_IF_NULL_PTR(ptr) do { \
    if (!ptr) return SLIDING_WINDOW_ERRCODE_NULL_PTR; \
}while(0)

#define RETURN_NULL_IF_NULL_PTR(ptr) do { \
    if (!ptr) return NULL; \
}while(0)

SLIDING_WINDOW *SlidingWindow_IMPL_new(SLIDING_WINDOW_THRESHOLD *cfg);
VOID SlidingWindow_IMPL_delete(SLIDING_WINDOW *window);
VOID SlidingWindow_IMPL_clear(SLIDING_WINDOW *window);
UINT32 SlidingWindow_IMPL_update(UINT32 hit, UINT32 curTick, SLIDING_WINDOW *window);

#endif