#ifndef __TWTK_CAIRO_UTIL_H
#define __TWTK_CAIRO_UTIL_H

#include <cairo.h>
#include <twtk/types.h>
#include <twtk/color.h>

static inline void _twtk_ut_set_rgba(cairo_t* cr, twtk_color_t rgba)
{
    /* note: cairo uses float numbers between 0..1 */
    cairo_set_source_rgba(cr, rgba.red, rgba.green, rgba.blue, rgba.alpha);
}

static inline int _twtk_ut_color_visible(twtk_color_t rgba)
{
    return rgba.alpha != 0;
}

cairo_surface_t *_twtk_ut_load_image_surface(const char* fn)
    __attribute__((nonnull(1)));

#endif /* __TWTK_CAIRO_UTIL_H */
