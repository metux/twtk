#ifndef __TWTK_VIEWPORT_H
#define __TWTK_VIEWPORT_H

#include <assert.h>
#include <cairo.h>
#include <twtk/types.h>
#include <twtk/rect.h>


void twtk_viewport_matrix(const twtk_rect_t viewport, cairo_matrix_t *matrix)
    __attribute__((nonnull(2)));

void twtk_viewport_inv_matrix(const twtk_rect_t viewport, cairo_matrix_t *matrix)
    __attribute__((nonnull(2)));

/**
 * translate a position from parent frame into viewport coordinate system
 */
int twtk_viewport_pos_from_frame(
    const twtk_rect_t viewport,
    twtk_vector_t pos,
    twtk_vector_t *ret_pos)
    __attribute__((nonnull(3)));

/**
 * translate a position from widget coordinate system to parent frame
 */
int twtk_viewport_pos_to_frame(
    const twtk_rect_t viewport,
    twtk_vector_t pos,
    twtk_vector_t *ret_pos)
    __attribute__((nonnull(3)));

#endif
