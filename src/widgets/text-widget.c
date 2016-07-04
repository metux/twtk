
#ifdef ENABLE_DEBUG_WIDGET_TEXT
#define _DEBUG_NAME	"text-widget"
#endif

#define _GNU_SOURCE

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <cairo.h>
#include <malloc.h>
#include <assert.h>
#include <twtk/widget.h>
#include <twtk/types.h>
#include <twtk/widget-helper.h>
#include <twtk-private/cairo_util.h>
#include <twtk-private/strutil.h>
#include <twtk-private/debug.h>


#define MAGIC			0x4554545830303030	// TEXT0000

#define _WIDGET_PRIV_TYPE	_priv_t
#define _WIDGET_CLASS_INF	_class_inf

typedef struct
{
    char* text;
    char* font_name;
    float font_size;
    float hspace;
    float vspace;
    twtk_color_t font_color;
    twtk_color_t border_color;
    int border_width;
} _priv_t;

static int _op_event(twtk_widget_t *widget, twtk_event_t *event, twtk_event_dispatch_t d)
    __attribute__((nonnull(1,2)));

static int _op_fini(twtk_widget_t *widget);
static int _op_paint(twtk_widget_t *widget, cairo_t *cr);
static int _op_set_color(twtk_widget_t *widget, const char* name, twtk_color_t value);
static int _op_set_float(twtk_widget_t *widget, const char* name, double value);
static int _op_set_str(twtk_widget_t *widget, const char* name, const char* value);
static int _op_set_uint(twtk_widget_t *widget, const char* name, uint32_t value);

static twtk_widget_class_t _class_inf = {
    .magic        = MAGIC,
    .op_event     = _op_event,
    .op_fini      = _op_fini,
    .op_paint     = _op_paint,
    .op_set_color = _op_set_color,
    .op_set_float = _op_set_float,
    .op_set_str   = _op_set_str,
    .op_set_uint  = _op_set_uint,
    .priv_size    = sizeof(_priv_t)
};

static int _op_event(twtk_widget_t *widget, twtk_event_t *event, twtk_event_dispatch_t d)
{
    TWTK_WIDGET_OP_BEGIN
    TWTK_WIDGET_OP_END(0);
}

static int _op_fini(twtk_widget_t *widget)
{
    TWTK_WIDGET_OP_BEGIN
    STRBUF_FREE(priv->font_name);
    TWTK_WIDGET_OP_END(0);
}

static int _op_paint(twtk_widget_t *widget, cairo_t *cr)
{
    TWTK_WIDGET_OP_BEGIN

    int w = widget->viewport.size.x;
    int h = widget->viewport.size.y;

    /* paint border */
    if (_twtk_ut_color_visible(priv->border_color))
    {
	_twtk_ut_set_rgba(cr, priv->border_color);
	cairo_set_line_width(cr, priv->border_width);
	cairo_rectangle (cr, 0, 0, w, h);
	cairo_stroke (cr);
    }

    /* paint text */
    if (_twtk_ut_color_visible(priv->font_color) && (priv->text != NULL))
    {
	_twtk_ut_set_rgba(cr, priv->font_color);
	cairo_select_font_face(cr, STRBUF_VALUE(priv->font_name), CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
	cairo_set_font_size(cr, priv->font_size);

	char *buffer = strdup(STRBUF_VALUE(priv->text));
	char *pch;
	pch = strtok (buffer, "\n");

	double offset = priv->font_size + priv->vspace;
	while (pch)
	{
	    cairo_move_to(cr, priv->hspace, offset);
	    cairo_show_text(cr, pch);
	    pch = strtok (NULL, "\n");
	    offset += priv->font_size;
	}

	free(buffer);
    }

    TWTK_WIDGET_OP_END(0);
}

static int _op_set_color(twtk_widget_t *widget, const char* name, twtk_color_t value)
{
    TWTK_WIDGET_SET_BEGIN
    TWTK_WIDGET_SET_COLOR_ATTR("border-color",	border_color);
    TWTK_WIDGET_SET_COLOR_ATTR("font-color",	font_color);
    TWTK_WIDGET_SET_END
}

static int _op_set_float(twtk_widget_t *widget, const char* name, double value)
{
    TWTK_WIDGET_SET_BEGIN
    TWTK_WIDGET_SET_FLOAT_ATTR("border-width",	border_width);
    TWTK_WIDGET_SET_FLOAT_ATTR("font-size",	font_size);
    TWTK_WIDGET_SET_FLOAT_ATTR("hspace",	hspace);
    TWTK_WIDGET_SET_FLOAT_ATTR("vspace",	vspace);
    TWTK_WIDGET_SET_END
}

static int _op_set_str(twtk_widget_t *widget, const char* name, const char* value)
{
    TWTK_WIDGET_SET_BEGIN
    TWTK_WIDGET_SET_STR_ATTR("text", 		text);
    TWTK_WIDGET_SET_STR_ATTR("font-name",	font_name);
    TWTK_WIDGET_SET_END
}

static int _op_set_uint(twtk_widget_t *widget, const char* name, uint32_t value)
{
    TWTK_WIDGET_SET_BEGIN
    TWTK_WIDGET_SET_INT_ATTR("border-width",	border_width);
    TWTK_WIDGET_SET_INT_ATTR("font-size",	font_size);
    TWTK_WIDGET_SET_INT_ATTR("hspace",		hspace);
    TWTK_WIDGET_SET_INT_ATTR("vspace",		vspace);
    TWTK_WIDGET_SET_END
}

twtk_widget_t *twtk_text_widget_create(const char* text, double x, double y, double w, double h)
{
    twtk_widget_t *widget = twtk_widget_alloc(&_class_inf);
    _priv_t *priv = (_priv_t *)(widget->priv);

    STRBUF_SET(priv->text, text);
    STRBUF_SET(priv->font_name, "monospace");

    twtk_widget_move(widget, x, y);
    twtk_widget_resize(widget, w, h);
    twtk_widget_vresize(widget, w, h);

    priv->font_size = 20.0;
    priv->border_width = 5;
    priv->hspace = 5;
    priv->vspace = 5;

    return widget;
}
