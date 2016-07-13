#ifndef __TWTK_PRIVATE_WIDGET_H
#define __TWTK_PRIVATE_WIDGET_H

#include <cairo.h>
#include <twtk/types.h>


cairo_matrix_t _twtk_widget_calc_matrix(const twtk_widget_t *widget);
cairo_matrix_t _twtk_widget_calc_inv_matrix(const twtk_widget_t *widget);

/**
 * translate a position from parent frame into widget coordinate system
 */
int twtk_widget_translate_pos_from_frame(const twtk_widget_t *widget,
			      twtk_vector_t pos,
			      twtk_vector_t *ret_pos)
    __attribute__((nonnull(1,3)));

/**
 * translate a position from widget coordinate system to parent frame
 */
int twtk_widget_translate_pos_to_frame(const twtk_widget_t *widget,
			      twtk_vector_t pos,
			      twtk_vector_t *ret_pos)
    __attribute__((nonnull(1,3)));

#endif /* __TWTK_PRIVATE_WIDGET_H */
