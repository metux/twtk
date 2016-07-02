
#include <string.h>
#include <cairo.h>
#include <cairo-jpeg.h>
#include <twtk-private/cairo_util.h>
#include <twtk-private/debug.h>

static inline int _twtk_ut_is_ext(const char* fn, const char* e)
{
    return (strcmp(&(fn[strlen(fn)-strlen(e)]), e)==0);
}

cairo_surface_t *_twtk_ut_load_image_surface(const char* fn)
{
    if (fn == NULL)
        return NULL;

    if (_twtk_ut_is_ext(fn, ".jpg")  ||
        _twtk_ut_is_ext(fn, ".JPG")  ||
        _twtk_ut_is_ext(fn, ".jpeg") ||
        _twtk_ut_is_ext(fn, ".JPEG"))
        return cairo_image_surface_create_from_jpeg(fn);

    if (_twtk_ut_is_ext(fn, ".png") ||
        _twtk_ut_is_ext(fn, ".PNG"))
        return cairo_image_surface_create_from_png(fn);

    _DEBUG("failed to detect image format: \"%s\"", fn);
    return NULL;
}
