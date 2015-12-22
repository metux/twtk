
#define _DEBUG_NAME	"window-widget"

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <cairo.h>
#include <assert.h>
#include <twtk/widget.h>
#include <twtk/types.h>
#include <twtk/events.h>
#include <twtk/widget-helper.h>
#include <twtk-private/cairo_util.h>
#include <twtk-private/strutil.h>
#include <twtk-private/debug.h>


#define MAGIC			0x57494E3157494E31      // WIN1WIN1

#define _WIDGET_PRIV_TYPE	_priv_t
#define _WIDGET_CLASS_INF	_class_inf

typedef struct
{
} _priv_t;

static int _op_event(twtk_widget_t *widget, twtk_event_t *event);

static twtk_widget_class_t _class_inf = {
    .magic        = MAGIC,
    .op_event     = _op_event,
    .priv_size    = sizeof(_priv_t)
};

static int _op_event(twtk_widget_t *widget, twtk_event_t *event)
{
    TWTK_WIDGET_OP_BEGIN

    if ((event->type == TWTK_EVENT_MOUSE) &&
	(event->mouse.event == TWTK_EVENT_MOUSE_MOVE))
    {
	if (event->mouse.all_buttons & TWTK_EVENT_MOUSE_BUTTON_MIDDLE)
	{
	    twtk_widget_move_rel(widget, event->mouse.relative.x, event->mouse.relative.y);
	    TWTK_WIDGET_OP_RETURN(TWTK_EVSTAT_CONSUMED | TWTK_EVSTAT_DIRTY);
	}
    }

    TWTK_WIDGET_OP_END(0);
}

twtk_widget_t *twtk_window_widget_create(double x, double y, double w, double h)
{
    twtk_widget_t *widget = twtk_widget_alloc(&_class_inf);
    _priv_t *priv = (_priv_t *)(widget->priv);

    twtk_widget_move(widget, x, y);
    twtk_widget_resize(widget, w, h);
    twtk_widget_vresize(widget, w, h);

    return widget;
}
