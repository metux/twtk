#ifndef __TWTK_VIEWPORT_H
#define __TWTK_VIEWPORT_H

#include <assert.h>
#include <cairo.h>
#include <twtk/types.h>
#include <twtk/rect.h>
#include <twtk/widget.h>


cairo_matrix_t *_twtk_do_viewport_matrix(twtk_widget_t *widget)
    __attribute__((nonnull(1)));

cairo_matrix_t *_twtk_do_viewport_inv_matrix(twtk_widget_t *widget)
    __attribute__((nonnull(1)));

static inline cairo_matrix_t *twtk_viewport_matrix(twtk_widget_t *widget)
{
    if (widget->flags & TWTK_WIDGET_FLAG_HAS_MATRIX)
        return &widget->matrix;

    return _twtk_do_viewport_matrix(widget);
}

static inline cairo_matrix_t *twtk_viewport_inv_matrix(twtk_widget_t *widget)
{
    if (widget->flags & TWTK_WIDGET_FLAG_HAS_INV_MATRIX)
        return &widget->inv_matrix;

    return _twtk_do_viewport_inv_matrix(widget);
}


/**
 * translate a position from parent frame into viewport coordinate system
 */
int twtk_viewport_pos_from_frame(
    twtk_widget_t *widget,
    twtk_vector_t pos,
    twtk_vector_t *ret_pos)
    __attribute__((nonnull(3)));

/**
 * translate a position from widget coordinate system to parent frame
 */
int twtk_viewport_pos_to_frame(
    twtk_widget_t *widget,
    twtk_vector_t pos,
    twtk_vector_t *ret_pos)
    __attribute__((nonnull(3)));

#endif
