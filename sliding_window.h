#ifndef __SLIDING_WINDOW_H__
#define __SLIDING_WINDOW_H__

#include "basic_types.h"

typedef struct
{
    USHORT slidingNum;
    UCHAR  continuousHitNum;
    UCHAR  hitNum;
}SLIDING_WINDOW_THRESHOLD;

VOID *SlidingWindow_new(SLIDING_WINDOW_THRESHOLD *cfg);
UINT32 SlidingWindow_delete(VOID *window);
UINT32 SlidingWindow_clear(VOID *window);
UINT32 SlidingWindow_update(UINT32 hit, UINT32 curTick, VOID *window);

#endif
