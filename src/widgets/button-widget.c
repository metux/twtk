
#define _DEBUG_NAME	"button-widget"

#ifdef ENABLE_DEBUG_WIDGET_BUTTON
#define ENABLE_DEBUG
#endif

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <cairo.h>
#include <math.h>
#include <assert.h>
#include <twtk/events.h>
#include <twtk/widget.h>
#include <twtk/types.h>
#include <twtk/widget-helper.h>
#include <twtk-private/cairo_util.h>
#include <twtk-private/strutil.h>
#include <twtk-private/debug.h>


#define MAGIC			0x554254544e4f3130      // "BUTTON01"

#define _WIDGET_PRIV_TYPE	_priv_t
#define _WIDGET_CLASS_INF	_class_inf

#define FRAME_COLOR		TWTK_COLOR_RED
#define FRAME_SIZE		4

typedef struct
{
    char* filename;
    twtk_color_t border_color;
    int border_width;
    cairo_surface_t *image;
    char *signal;
    char down:1;
} _priv_t;

static int _op_fini(twtk_widget_t *widget);
static int _op_paint(twtk_widget_t *widget, cairo_t *cr);
static int _op_event(twtk_widget_t *widget, twtk_event_t *event, twtk_event_dispatch_t d);
static int _op_set_float(twtk_widget_t *widget, const char* name, double value);
static int _op_set_uint(twtk_widget_t *widget, const char* name, uint32_t value);
static int _op_set_color(twtk_widget_t *widget, const char* name, twtk_color_t value);

static const twtk_widget_class_t _class_inf = {
    .magic        = MAGIC,
    .op_fini      = _op_fini,
    .op_event     = _op_event,
    .op_paint     = _op_paint,
    .op_set_float = _op_set_float,
    .op_set_uint  = _op_set_uint,
    .op_set_color = _op_set_color,
    .priv_size    = sizeof(_priv_t)
};

static int _op_fini(twtk_widget_t *widget)
{
    TWTK_WIDGET_OP_BEGIN
    cairo_surface_destroy(priv->image);
    STRBUF_FREE(priv->signal);
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
    if (priv->down)
    {
        _twtk_ut_set_rgba (cr, priv->border_color);
        cairo_set_line_width (cr, priv->border_width);
        _twtk_ut_rect_to_vec (cr, widget->viewport.size);
        cairo_stroke (cr);
    }

    TWTK_WIDGET_OP_END(0);
}

static int _op_event_mouse(twtk_widget_t *widget, twtk_event_t *event, twtk_event_dispatch_t d)
{
    assert(widget);
    assert(event);

    TWTK_WIDGET_OP_BEGIN

    switch (event->mouse.event)
    {
        case TWTK_EVENT_MOUSE_LEAVE:
            _DEBUG("mouse leave");
            priv->down = 0;
            TWTK_WIDGET_OP_RETURN(TWTK_EVSTAT_CONSUMED | TWTK_EVSTAT_DIRTY);
        break;

        case TWTK_EVENT_MOUSE_BTN_DOWN:
            if (event->mouse.button == TWTK_EVENT_MOUSE_BUTTON_LEFT)
            {
                _DEBUG("click dn @ %f:%f", event->mouse.pos.x, event->mouse.pos.y);
                priv->down = 1;
                TWTK_WIDGET_OP_RETURN(TWTK_EVSTAT_CONSUMED | TWTK_EVSTAT_DIRTY);
            }
        break;

        case TWTK_EVENT_MOUSE_BTN_UP:
            if (event->mouse.button == TWTK_EVENT_MOUSE_BUTTON_LEFT)
            {
                _DEBUG("click up @ %f:%f", event->mouse.pos.x, event->mouse.pos.y);
                priv->down = 0;
                twtk_widget_notify_action(widget, TWTK_EVENT_ACTION_PUSH, priv->signal);
                TWTK_WIDGET_OP_RETURN(TWTK_EVSTAT_CONSUMED | TWTK_EVSTAT_DIRTY);
            }
        break;

        case TWTK_EVENT_MOUSE_MOVE:
            TWTK_WIDGET_OP_RETURN(0);
        break;

        default:
            _DEBUG("unhandled mouse event %s @ %s", twtk_event_mouse_type_name(event->mouse.event), widget->name);
        break;
    }

    TWTK_WIDGET_OP_END(0);
}

static int _op_event(twtk_widget_t *widget, twtk_event_t *event, twtk_event_dispatch_t d)
{
    assert(widget);
    assert(event);

    if (event->type == TWTK_EVENT_MOUSE)
        return _op_event_mouse(widget, event, d);

    _DEBUG("non-mouse event: %s", twtk_event_type_name(event->type));

    return 0;
}

static int _op_set_color(twtk_widget_t *widget, const char* name, twtk_color_t value)
{
    TWTK_WIDGET_SET_BEGIN
    TWTK_WIDGET_SET_COLOR_ATTR("border-color",  border_color);
    TWTK_WIDGET_SET_END
}

static int _op_set_float(twtk_widget_t *widget, const char* name, double value)
{
    TWTK_WIDGET_SET_BEGIN
    TWTK_WIDGET_SET_FLOAT_ATTR("border-width",  border_width);
    TWTK_WIDGET_SET_END
}

static int _op_set_uint(twtk_widget_t *widget, const char* name, uint32_t value)
{
    TWTK_WIDGET_SET_BEGIN
    TWTK_WIDGET_SET_INT_ATTR("border-width",    border_width);
    TWTK_WIDGET_SET_END
}

twtk_widget_t *twtk_button_widget_create(const char* fn, twtk_rect_t rect, const char *signal)
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
    priv->border_width = FRAME_SIZE;
    priv->border_color = FRAME_COLOR;
    STRBUF_SET(priv->signal, signal);

    twtk_widget_set_viewport(
        widget,
        twtk_rect_size_if_null_d(
            rect,
            cairo_image_surface_get_width (priv->image),
            cairo_image_surface_get_height (priv->image)
        )
    );

    twtk_widget_vresize(widget, rect.size);

    return widget;
}
