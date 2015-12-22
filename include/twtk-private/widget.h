#ifndef __TWTK_PRIVATE_WIDGET_H
#define __TWTK_PRIVATE_WIDGET_H

#include <cairo.h>
#include <twtk/types.h>

cairo_matrix_t _twtk_widget_calc_matrix(const twtk_widget_t *widget);
cairo_matrix_t _twtk_widget_calc_inv_matrix(const twtk_widget_t *widget);

int twtk_widget_translate_pos(const twtk_widget_t *widget,
			      twtk_vector_t pos,
			      twtk_vector_t *ret_pos);

#endif /* __TWTK_PRIVATE_WIDGET_H */
