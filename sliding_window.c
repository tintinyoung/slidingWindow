#include "sliding_window.h"
#include "sliding_window_priv.h"
#include <string.h>
#include <stdlib.h>

INTERFACE VOID *SlidingWindow_new(SLIDING_WINDOW_THRESHOLD *threshold)
{
    RETURN_NULL_IF_NULL_PTR(threshold);
    return SlidingWindow_IMPL_new(threshold);
}

INTERFACE UINT32 SlidingWindow_delete(VOID *window)
{
    RETURN_ERR_IF_NULL_PTR(window);
    SlidingWindow_IMPL_delete((SLIDING_WINDOW *)window);
    return SLIDING_WINOW_ECCODE_SUCCESSFUL;
}

INTERFACE UINT32 SlidingWindow_clear(VOID *window)
{
    RETURN_ERR_IF_NULL_PTR(window);
    SlidingWindow_IMPL_clear((SLIDING_WINDOW *)window);
    return SLIDING_WINOW_ECCODE_SUCCESSFUL;
}

INTERFACE UINT32 SlidingWindow_update(UINT32 hit, UINT32 curTick, VOID *window)
{
    RETURN_ERR_IF_NULL_PTR(window);
    return SlidingWindow_IMPL_update(hit, curTick, (SLIDING_WINDOW *)window);
}

SLIDING_WINDOW *SlidingWindow_IMPL_new(SLIDING_WINDOW_THRESHOLD *threshold)
{
    SLIDING_WINDOW *window = malloc(SLIDING_WINDOW_SIZE(threshold->slidingNum));
    (VOID)memcpy(&(window->threshold), threshold, sizeof(SLIDING_WINDOW_THRESHOLD));
    (VOID)memset(&(window->hitTicks), 0, SLIDING_RING_SIZE(threshold->slidingNum));
    return window;
}

VOID SlidingWindow_IMPL_delete(SLIDING_WINDOW *window)
{
    free(window);
}

VOID SlidingWindow_IMPL_clear(SLIDING_WINDOW *window)
{
    window->hitTicks.head = 0;
    window->hitTicks.tail = 0;
    (VOID)memset(window->hitTicks.values, 0, 
                sizeof(window->hitTicks.values[0]) * window->threshold.slidingNum);
}
UINT32 SlidingWindow_IMPL_update(UINT32 hit, UINT32 curTick, SLIDING_WINDOW *window)
{
    RING *ticks = &(window->hitTicks);

    if (distance(RING_get(RING_tail(ticks), ticks), curTick) >= window->threshold.slidingNum)
    {
        RING_remove(ticks);
    }
    
    if (!hit)
    {
        return FALSE;
    }

    RING_add(curTick, ticks);

    if (SlidingWindow_IMPL_meetContinuousTicks(curTick, window))
    {
        return TRUE;
    }

    if (RING_size(ticks) >= window->threshold.hitNum)
    {
        return TRUE;
    }

    return FALSE;
}

UINT32 SlidingWindow_IMPL_meetContinuousTicks(UINT32 curTick, SLIDING_WINDOW *window)
{
    UINT32 i;
    UINT32 continuousCount = 0;
    UINT32 prevTick = curTick;
    RING *ticks = &(window->hitTicks);

    for (i = RING_head(ticks); i != RING_tail(ticks); i = RING_prev(i, ticks))
    {
        UINT32 tick = RING_get(i, ticks);
        if ((tick + 1 == prevTick) || (tick == prevTick))
        {
            ++continuousCount;
        }
        else
        {
            break;
        }

        if (continuousCount == window->threshold.continuousHitNum)
        {
            return TRUE;
        }
    }

    return FALSE;
}

UINT32 distance(UINT32 x, UINT32 y)
{
    if (x <= y)
    {
        return y - x;
    }
    else
    {
        return 0xffffffff - x + y + 1;
    }
}

VOID RING_add(UINT32 value, RING *ring)
{
    ring->values[ring->head] = value;
    if ((!RING_is_empty(ring)) && (ring->head == ring->tail))
    {
        ring->tail = RING_incr(ring->tail, ring);
    }
    ring->head = RING_incr(ring->head, ring);
    ++ring->size;
}

UINT32 RING_is_empty(RING *ring)
{
    if (ring->size == 0)
    {
        return TRUE;
    } 
    else
    {
        return FALSE;
    }
}

UINT32 ring_is_full(RING *ring)
{
    if (RING_size(ring) == ring->length)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

UINT32 RING_size(RING *ring)
{
    return ring->size;
}

UINT32 RING_head(RING *ring)
{
    return ring->head;
}

UINT32 RING_tail(RING *ring)
{
    return ring->tail;
}

UINT32 RING_prev(UINT32 index, RING *ring)
{
    return RING_decr(index, ring);
}

UINT32 RING_next(UINT32 index, RING *ring)
{
    return RING_incr(index, ring);
}

UINT32 RING_get(UINT32 index, RING *ring)
{
    return ring->values[index];
}

UINT32 RING_incr(USHORT index, RING *ring)
{
    return (index + 1) % ring->length;
}

UINT32 RING_decr(USHORT index, RING *ring)
{
    if (index != 0)
    {
        return index - 1;
    }
    else
    {
        return ring->length - 1;
    }
}