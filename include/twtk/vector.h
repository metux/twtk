#ifndef __TWTK_VECTOR_H_
#define __TWTK_VECTOR_H_

#include <math.h>
#include <stdio.h>
#include <twtk/types.h>

struct __twtk_vector
{
    twtk_dim_t x;
    twtk_dim_t y;
};

#define TWTK_VECTOR(_x,_y)	((twtk_vector_t){ .x = _x, .y = _y })
#define TWTK_VECTOR_ZERO	((twtk_vector_t){ .x =  0, .y =  0 })

static inline twtk_vector_t twtk_vector_mul(twtk_vector_t vec, twtk_dim_t factor)
{
    return (twtk_vector_t){
	.x = vec.x * factor,
	.y = vec.y * factor
    };
}

static inline twtk_vector_t twtk_vector_add(twtk_vector_t vec1, twtk_vector_t vec2)
{
    return (twtk_vector_t){
	.x = vec1.x + vec2.x,
	.y = vec1.y + vec2.y
    };
}

static inline twtk_vector_t twtk_vector_sub(twtk_vector_t vec1, twtk_vector_t vec2)
{
    return (twtk_vector_t){
	.x = vec1.x - vec2.x,
	.y = vec1.y - vec2.y
    };
}

static inline twtk_vector_t twtk_vector_rot(twtk_vector_t vec, twtk_dim_t angle)
{
    twtk_dim_t sin_a = sin(angle);
    twtk_dim_t cos_a = cos(angle);

    return (twtk_vector_t){
	.x = cos_a * vec.x - sin_a * vec.y,
	.y = sin_a * vec.x + cos_a * vec.y
    };
}

#endif /* __TWTK_VECTOR_H_ */
