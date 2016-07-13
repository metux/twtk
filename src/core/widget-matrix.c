
#include <assert.h>
#include <twtk/types.h>
#include <twtk/widget.h>
#include <twtk-private/widget.h>

cairo_matrix_t _twtk_widget_calc_matrix(const twtk_widget_t *widget)
{
    cairo_matrix_t matrix;
    cairo_matrix_init_identity(&matrix);

    cairo_matrix_translate(&matrix, widget->viewport.pos.x, widget->viewport.pos.y);
    if (widget->viewport.angle)
    {
	twtk_vector_t offset = twtk_vector_mul(widget->viewport.size, 0.5);
	cairo_matrix_translate(&matrix, offset.x, offset.y);
	cairo_matrix_rotate(&matrix, widget->viewport.angle);
	cairo_matrix_translate(&matrix, - offset.x, - offset.y);
    }

    return matrix;
}

cairo_matrix_t _twtk_widget_calc_inv_matrix(const twtk_widget_t *widget)
{
    cairo_matrix_t matrix;
    cairo_matrix_init_identity(&matrix);

    if (widget->viewport.angle)
    {
	twtk_vector_t offset = twtk_vector_mul(widget->viewport.size, 0.5);
	cairo_matrix_translate(&matrix, offset.x, offset.y);
	cairo_matrix_rotate(&matrix, -widget->viewport.angle);
	cairo_matrix_translate(&matrix, -offset.x, -offset.y);
    }
    cairo_matrix_translate(&matrix, -widget->viewport.pos.x, -widget->viewport.pos.y);

    return matrix;
}

/**
 * translate a position from parent frame into widget coordinate system
 */
int twtk_widget_translate_pos_from_frame(
    const twtk_widget_t *widget,
    twtk_vector_t pos,
    twtk_vector_t *ret_pos)
{
    assert(widget);
    assert(ret_pos);

    cairo_matrix_t matrix = _twtk_widget_calc_inv_matrix(widget);
    cairo_matrix_transform_point(&matrix, &pos.x, &pos.y);

    *ret_pos = pos;

    return twtk_vector_in_rect(
        pos,
        twtk_rect_by_vectors(TWTK_VECTOR(0,0), widget->viewport.size, 0)
    );
}

/**
 * translate a position from widget coordinate system to parent frame
 */
int twtk_widget_translate_pos_to_frame(
    const twtk_widget_t *widget,
    twtk_vector_t pos,
    twtk_vector_t *ret_pos)
{
    assert(widget);
    assert(ret_pos);

    cairo_matrix_t matrix = _twtk_widget_calc_matrix(widget);
    cairo_matrix_transform_point(&matrix, &pos.x, &pos.y);

    *ret_pos = pos;

    return 1;
}
