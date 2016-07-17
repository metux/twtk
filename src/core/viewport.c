
#include <assert.h>
#include <cairo.h>
#include <twtk/types.h>
#include <twtk/rect.h>
#include <twtk/viewport.h>


void twtk_viewport_matrix(const twtk_rect_t viewport, cairo_matrix_t *matrix)
{
    assert(matrix);
    cairo_matrix_init_identity(matrix);
    cairo_matrix_translate(matrix, viewport.pos.x, viewport.pos.y);

    if (viewport.angle)
    {
	twtk_vector_t offset = twtk_vector_mul(viewport.size, 0.5);
	cairo_matrix_translate(matrix, offset.x, offset.y);
	cairo_matrix_rotate(matrix, viewport.angle);
	cairo_matrix_translate(matrix, - offset.x, - offset.y);
    }
}

void twtk_viewport_inv_matrix(const twtk_rect_t viewport, cairo_matrix_t *matrix)
{
    cairo_matrix_init_identity(matrix);

    if (viewport.angle)
    {
	twtk_vector_t offset = twtk_vector_mul(viewport.size, 0.5);
	cairo_matrix_translate(matrix, offset.x, offset.y);
	cairo_matrix_rotate(matrix, -viewport.angle);
	cairo_matrix_translate(matrix, -offset.x, -offset.y);
    }

    cairo_matrix_translate(matrix, -viewport.pos.x, -viewport.pos.y);
}

/**
 * translate a position from parent frame into viewport coordinate system
 */
int twtk_viewport_pos_from_frame(
    const twtk_rect_t viewport,
    twtk_vector_t pos,
    twtk_vector_t *ret_pos)
{
    assert(ret_pos);

    cairo_matrix_t matrix;
    twtk_viewport_inv_matrix(viewport, &matrix);
    cairo_matrix_transform_point(&matrix, &pos.x, &pos.y);

    *ret_pos = pos;

    return twtk_vector_in_rect(
        pos,
        twtk_rect_by_vectors(TWTK_VECTOR(0,0), viewport.size, 0)
    );
}

/**
 * translate a position from widget coordinate system to parent frame
 */
int twtk_viewport_pos_to_frame(
    const twtk_rect_t viewport,
    twtk_vector_t pos,
    twtk_vector_t *ret_pos)
{
    assert(ret_pos);

    cairo_matrix_t matrix;
    twtk_viewport_matrix(viewport, &matrix);
    cairo_matrix_transform_point(&matrix, &pos.x, &pos.y);

    *ret_pos = pos;

    return 1;
}
