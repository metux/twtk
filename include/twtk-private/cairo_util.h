#ifndef __TWTK_CAIRO_UTIL_H
#define __TWTK_CAIRO_UTIL_H

#include <errno.h>
#include <cairo.h>
#include <twtk/types.h>
#include <twtk/color.h>
#include <twtk/rect.h>
#include <twtk-private/debug.h>


static inline void _twtk_ut_set_rgba(cairo_t* cr, twtk_color_t rgba)
{
    /* note: cairo uses float numbers between 0..1 */
    cairo_set_source_rgba(cr, rgba.red, rgba.green, rgba.blue, rgba.alpha);
}

static inline int _twtk_ut_color_visible(twtk_color_t rgba)
{
    return rgba.alpha != 0;
}

static inline void _twtk_ut_rect(cairo_t *cr, twtk_rect_t rect)
{
    cairo_rectangle(cr,
        rect.pos.x,
        rect.pos.y,
        rect.size.x,
        rect.size.y
    );
}

static inline void _twtk_ut_rect_to_vec(cairo_t *cr, twtk_vector_t vec)
{
    cairo_rectangle(cr, 0, 0, vec.x, vec.y);
}

static inline int _twtk_ut_paint_surface(cairo_t *cr, cairo_surface_t *surface, twtk_vector_t pos)
{
    if (cr == NULL) return -EFAULT;
    if (cairo_surface_status(surface) != CAIRO_STATUS_SUCCESS) {
        _DEBUG("_twtk_ut_paint_surface: invalid source surface");
        return -EINVAL;
    }

    cairo_set_source_surface (cr, surface, pos.x, pos.y);
    cairo_paint (cr);

    return 0;
}

cairo_surface_t *_twtk_ut_load_image_surface(const char* fn)
    __attribute__((nonnull(1)));

#endif /* __TWTK_CAIRO_UTIL_H */
