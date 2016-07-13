
#define _DEBUG_NAME	"pattern-widget"

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <cairo.h>
#include <assert.h>
#include <twtk/widget.h>
#include <twtk/types.h>
#include <twtk/widget-helper.h>
#include <twtk-private/strutil.h>
#include <twtk-private/debug.h>


#define MAGIC			0x4d49474130453130      // "IMAGE001"

#define _WIDGET_PRIV_TYPE	_priv_t
#define _WIDGET_CLASS_INF	_class_inf

typedef struct
{
} _priv_t;

static int _op_event(twtk_widget_t *widget, twtk_event_t *event);
static int _op_fini(twtk_widget_t *widget);
static int _op_paint(twtk_widget_t *widget, cairo_t *cr);
static int _op_set_color(twtk_widget_t *widget, const char* name, twtk_color_t value);
static int _op_set_float(twtk_widget_t *widget, const char* name, double value);
static int _op_set_str(twtk_widget_t *widget, const char* name, const char* value);
static int _op_set_uint(twtk_widget_t *widget, const char* name, uint32_t value);

static twtk_widget_class_t _class_inf = {
    .magic        = MAGIC,
    .op_paint     = _op_paint,
    .priv_size    = sizeof(_priv_t)
};

static int _op_paint(twtk_widget_t *widget, cairo_t *cr)
{
    TWTK_WIDGET_OP_BEGIN

    cairo_pattern_t *linpat, *radpat;
    linpat = cairo_pattern_create_linear (0, 0, 1, 1);
    cairo_pattern_add_color_stop_rgb (linpat, 0, 0, 0.3, 0.8);
    cairo_pattern_add_color_stop_rgb (linpat, 1, 0, 0.8, 0.3);

    radpat = cairo_pattern_create_radial (0.5, 0.5, 0.25, 0.5, 0.5, 0.75);
    cairo_pattern_add_color_stop_rgba (radpat, 0, 0, 0, 0, 1);
    cairo_pattern_add_color_stop_rgba (radpat, 0.5, 0, 0, 0, 0);

    cairo_set_source (cr, linpat);
    cairo_mask (cr, radpat);

    TWTK_WIDGET_OP_END(0);
}

twtk_widget_t *twtk_pattern_widget_create(twtk_dim_t x, twtk_dim_t y, twtk_dim_t w, twtk_dim_t h)
{
    twtk_widget_t *widget = twtk_widget_alloc(&_class_inf);

    twtk_widget_move_coords(widget, x, y);
    twtk_widget_vresize(widget, w, h);

    return widget;
}
