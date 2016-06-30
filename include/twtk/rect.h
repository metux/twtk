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
    double angle;
};

static inline twtk_rect_t twtk_rect_by_vectors(
    twtk_vector_t pos,
    twtk_vector_t size,
    float angle)
{
    return ((twtk_rect_t){
        .pos   = pos,
        .size  = size,
        .angle = angle
    });
}

static inline twtk_rect_t twtk_rect_null()
{
    return (twtk_rect_t){ 0 };
}

#endif /* __TWTK_RECT_H_ */
