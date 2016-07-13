
#define _DEBUG_NAME	"image-widget"

#ifdef ENABLE_DEBUG_WIDGET_IMAGE
#define ENABLE_DEBUG
#endif

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <cairo.h>
#include <cairo-jpeg.h>
#include <math.h>
#include <assert.h>
#include <twtk/widget.h>
#include <twtk/types.h>
#include <twtk/widget-helper.h>
#include <twtk-private/cairo_util.h>
#include <twtk-private/strutil.h>
#include <twtk-private/debug.h>


#define MAGIC			0x4d49474130453130      // "IMAGE001"

#define _WIDGET_PRIV_TYPE	_priv_t
#define _WIDGET_CLASS_INF	_class_inf

typedef struct
{
    char* filename;
    twtk_color_t border_color;
    int border_width;
    cairo_surface_t *image;
} _priv_t;

static int _op_fini(twtk_widget_t *widget);
static int _op_paint(twtk_widget_t *widget, cairo_t *cr);
static int _op_set_color(twtk_widget_t *widget, const char* name, twtk_color_t value);
static int _op_set_float(twtk_widget_t *widget, const char* name, double value);
static int _op_set_str(twtk_widget_t *widget, const char* name, const char* value);
static int _op_set_uint(twtk_widget_t *widget, const char* name, uint32_t value);

static twtk_widget_class_t _class_inf = {
    .magic        = MAGIC,
    .op_fini      = _op_fini,
    .op_paint     = _op_paint,
    .op_set_color = _op_set_color,
    .op_set_float = _op_set_float,
    .op_set_str   = _op_set_str,
    .op_set_uint  = _op_set_uint,
    .priv_size    = sizeof(_priv_t)
};

static int _op_fini(twtk_widget_t *widget)
{
    TWTK_WIDGET_OP_BEGIN
    cairo_surface_destroy(priv->image);
    TWTK_WIDGET_OP_END(0);
}

static int _op_paint(twtk_widget_t *widget, cairo_t *cr)
{
    TWTK_WIDGET_OP_BEGIN

    assert (cr);
    assert (priv->image);

    cairo_set_source_surface (cr, priv->image, 0, 0);
    cairo_paint (cr);

    /* paint border */
    if (priv->border_color.alpha)
    {
	_twtk_ut_set_rgba (cr, priv->border_color);
	cairo_set_line_width (cr, priv->border_width);
        _twtk_ut_rect_to_vec (cr, widget->viewport.size);
	cairo_stroke (cr);
    }

    TWTK_WIDGET_OP_END(0);
}

static int _op_set_color(twtk_widget_t *widget, const char* name, twtk_color_t value)
{
    TWTK_WIDGET_SET_BEGIN
    TWTK_WIDGET_SET_COLOR_ATTR("border-color",	border_color);
    TWTK_WIDGET_SET_END
}

static int _op_set_float(twtk_widget_t *widget, const char* name, double value)
{
    TWTK_WIDGET_SET_BEGIN
    TWTK_WIDGET_SET_FLOAT_ATTR("border-width",	border_width);
    TWTK_WIDGET_SET_END
}

static int _op_set_str(twtk_widget_t *widget, const char* name, const char* value)
{
    TWTK_WIDGET_SET_BEGIN
    TWTK_WIDGET_SET_END
}

static int _op_set_uint(twtk_widget_t *widget, const char* name, uint32_t value)
{
    TWTK_WIDGET_SET_BEGIN
    TWTK_WIDGET_SET_INT_ATTR("border-width",	border_width);
    TWTK_WIDGET_SET_END
}

twtk_widget_t *twtk_image_widget_create(const char* fn, twtk_dim_t x, twtk_dim_t y, twtk_dim_t w, twtk_dim_t h)
{
    cairo_surface_t *img = _twtk_ut_load_image_surface(fn);

    if (img == NULL)
    {
	_DEBUG("failed to load image: \"%s\"", fn);
	return NULL;
    }

    twtk_widget_t *widget = twtk_widget_alloc(&_class_inf);
    _priv_t *priv = (_priv_t*)(widget->priv);

    priv->image = img;

    int img_w = cairo_image_surface_get_width (priv->image);
    int img_h = cairo_image_surface_get_height (priv->image);

    widget->viewport.size = TWTK_VECTOR ((w==0) ? img_w : w, (h==0) ? img_h : h);

    twtk_widget_move(widget, x, y);
    twtk_widget_vresize(widget, img_w, img_h);

    priv->border_width = 1;
    priv->border_color = TWTK_COLOR_RED;

    return widget;
}
