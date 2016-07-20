
#include <assert.h>
#include <cairo.h>
#include <twtk/types.h>
#include <twtk/rect.h>
#include <twtk/viewport.h>
#include <twtk/widget.h>


cairo_matrix_t *_twtk_do_viewport_matrix(twtk_widget_t *widget)
{
    if (widget->flags & TWTK_WIDGET_FLAG_HAS_MATRIX)
        return &widget->matrix;

    cairo_matrix_init_identity(&widget->matrix);
    cairo_matrix_translate(&widget->matrix, widget->viewport.pos.x, widget->viewport.pos.y);

    if (widget->viewport.angle)
    {
	twtk_vector_t offset = twtk_vector_mul(widget->viewport.size, 0.5);
	cairo_matrix_translate(&widget->matrix, offset.x, offset.y);
	cairo_matrix_rotate(&widget->matrix, widget->viewport.angle);
	cairo_matrix_translate(&widget->matrix, - offset.x, - offset.y);
    }

    widget->flags |= TWTK_WIDGET_FLAG_HAS_MATRIX;

    return &widget->matrix;
}

cairo_matrix_t *_twtk_do_viewport_inv_matrix(twtk_widget_t *widget)
{
    if (widget->flags & TWTK_WIDGET_FLAG_HAS_INV_MATRIX)
        return &widget->inv_matrix;

    cairo_matrix_init_identity(&widget->inv_matrix);

    if (widget->viewport.angle)
    {
	twtk_vector_t offset = twtk_vector_mul(widget->viewport.size, 0.5);
	cairo_matrix_translate(&widget->inv_matrix, offset.x, offset.y);
	cairo_matrix_rotate(&widget->inv_matrix, -(widget->viewport.angle));
	cairo_matrix_translate(&widget->inv_matrix, -offset.x, -offset.y);
    }

    cairo_matrix_translate(&widget->inv_matrix, -(widget->viewport.pos.x), -(widget->viewport.pos.y));

    widget->flags |= TWTK_WIDGET_FLAG_HAS_INV_MATRIX;

    return &widget->inv_matrix;
}

/**
 * translate a position from parent frame into viewport coordinate system
 */
int twtk_viewport_pos_from_frame(
    twtk_widget_t *widget,
    twtk_vector_t pos,
    twtk_vector_t *ret_pos)
{
    assert(ret_pos);

    cairo_matrix_t *matrix = twtk_viewport_inv_matrix(widget);
    cairo_matrix_transform_point(matrix, &pos.x, &pos.y);

    *ret_pos = pos;

    return twtk_vector_in_rect(
        pos,
        twtk_rect_by_vectors(TWTK_VECTOR(0,0), widget->viewport.size, 0)
    );
}

/**
 * translate a position from widget coordinate system to parent frame
 */
int twtk_viewport_pos_to_frame(
    twtk_widget_t *widget,
    twtk_vector_t pos,
    twtk_vector_t *ret_pos)
{
    assert(ret_pos);

    cairo_matrix_t *matrix = twtk_viewport_matrix(widget);
    cairo_matrix_transform_point(matrix, &pos.x, &pos.y);

    *ret_pos = pos;

    return 1;
}
