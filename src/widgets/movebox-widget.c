
#define _DEBUG_NAME	"movebox-widget"

#ifdef ENABLE_DEBUG_WIDGET_MOVEBOX
#define ENABLE_DEBUG
#endif

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

static int _op_event(twtk_widget_t *widget, twtk_event_t *event)
    __attribute__((nonnull(1,2)));

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
	    twtk_widget_move_rel(widget, event->mouse.diff.x, event->mouse.diff.y);
	    TWTK_WIDGET_OP_RETURN(TWTK_EVSTAT_CONSUMED | TWTK_EVSTAT_DIRTY);
	}
    }

    TWTK_WIDGET_OP_END(0);
}

twtk_widget_t *twtk_movebox_widget_create(twtk_widget_t *child)
{
    twtk_widget_t *movebox = twtk_widget_alloc(&_class_inf);

    twtk_rect_t rect = child->viewport;

    twtk_widget_move(movebox, rect.pos.x, rect.pos.y);
    twtk_widget_resize(movebox, rect.size.x, rect.size.y);
    twtk_widget_vresize(movebox, rect.size.x, rect.size.y);

    twtk_widget_move(child, 0, 0);

    twtk_widget_add_child(movebox, child, "sub0");

    return movebox;
}
