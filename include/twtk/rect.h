#ifndef __TWTK_RECT_H_
#define __TWTK_RECT_H_

#include <math.h>
#include <stdio.h>
#include <twtk/types.h>
#include <twtk/vector.h>


struct __twtk_rect
{
    twtk_vector_t pos;
    twtk_vector_t size;
    twtk_dim_t angle;
};

static inline twtk_rect_t twtk_rect_by_vectors(
    twtk_vector_t pos,
    twtk_vector_t size,
    twtk_dim_t angle)
{
    return ((twtk_rect_t){
        .pos   = pos,
        .size  = size,
        .angle = angle
    });
}

static inline twtk_rect_t twtk_rect_by_coords(
    twtk_dim_t x1,
    twtk_dim_t y1,
    twtk_dim_t x2,
    twtk_dim_t y2,
    twtk_dim_t angle
) {
    return twtk_rect_by_vectors(
        TWTK_VECTOR(x1, y1),
        TWTK_VECTOR(x2, y2),
        angle
    );
}

static inline twtk_rect_t twtk_rect_by_pos(
    twtk_dim_t x,
    twtk_dim_t y
) {
    return twtk_rect_by_vectors(
        TWTK_VECTOR(x, y),
        TWTK_VECTOR(-1, -1),
        0
    );
}

static inline twtk_rect_t twtk_rect_null()
{
    return (twtk_rect_t){ .size = { .x = -1, .y = -1 } };
}

static inline char twtk_vector_in_rect(twtk_vector_t vec, twtk_rect_t rect)
{
    return (
        (vec.x >= rect.pos.x) &&
        (vec.x <= (rect.pos.x + rect.size.x)) &&
        (vec.y >= rect.pos.y) &&
        (vec.y <= (rect.pos.y + rect.size.y)));
}

/**
 * set the rect size if null (-1,-1) - otherwise leave it untouched
 */
static inline twtk_rect_t twtk_rect_size_if_null_d(twtk_rect_t rect, twtk_dim_t x, twtk_dim_t y)
{
    if (rect.size.x == -1)
        rect.size.x = x;

    if (rect.size.y == -1)
        rect.size.y = y;

    return rect;
}

static inline twtk_rect_t twtk_rect_size_if_null_v(twtk_rect_t rect, twtk_vector_t vec)
{
    return twtk_rect_size_if_null_d(rect, vec.x, vec.y);
}

#endif /* __TWTK_RECT_H_ */
