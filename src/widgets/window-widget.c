
#define _DEBUG_NAME	"window-widget"

#ifdef ENABLE_DEBUG_WIDGET_WINDOW
#define ENABLE_DEBUG
#endif

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <cairo.h>
#include <assert.h>
#include <twtk/platform.h>
#include <twtk/widget.h>
#include <twtk/types.h>
#include <twtk/events.h>
#include <twtk/widget-helper.h>
#include <twtk/widgets/button.h>
#include <twtk/widgets/window.h>
#include <twtk-private/cairo_util.h>
#include <twtk-private/strutil.h>
#include <twtk-private/debug.h>


#define WINDOW_MAGIC			0x4957314e4957314e      // WIN1WIN1
#define WINDOW_BORDER_SIZE		2
#define WINDOW_BORDER_COLOR		TWTK_COLOR_RED
#define WINDOW_BORDER_TOP		WINDOW_BORDER_SIZE
#define WINDOW_BORDER_BOTTOM		WINDOW_BORDER_SIZE
#define WINDOW_BORDER_LEFT		WINDOW_BORDER_SIZE
#define WINDOW_BORDER_RIGHT		WINDOW_BORDER_SIZE
#define WINDOW_BACKGROUND_COLOR		TWTK_COLOR_BROWN
#define WINDOW_TITLEBAR_HEIGHT		20

#define SUBFRAME_MAGIC			0x4957314e55533142      // WIN1SUB1
#define SUBFRAME_BORDER_SIZE		1
#define SUBFRAME_BACKGROUND_COLOR	TWTK_COLOR_GREEN
#define SUBFRAME_BORDER_COLOR		TWTK_COLOR_BLACK
#define SUBFRAME_NAME			"client-area"
#define SUBFRAME_TOP			(WINDOW_BORDER_SIZE*2+WINDOW_TITLEBAR_HEIGHT)

#define CLOSEBTN_NAME			"button-close"
#define CLOSEBTN_IMAGE			"resources/button-cross.png"

typedef struct
{
    twtk_widget_t *client_area;
    twtk_widget_t *close_button;
} _priv_t;

static int _window_op_event(twtk_widget_t *widget, twtk_event_t *event, twtk_event_dispatch_t d)
    __attribute__((nonnull(1,2)));

static int _window_op_insert_child(twtk_widget_t *widget, twtk_widget_t *child)
    __attribute__((nonnull(1,2)));

static const twtk_widget_class_t _class_inf_window = {
    .magic           = WINDOW_MAGIC,
    .op_event        = _window_op_event,
    .op_insert_child = _window_op_insert_child,
    .priv_size       = sizeof(_priv_t)
};

static twtk_widget_class_t _class_inf_subframe = {
    .magic           = SUBFRAME_MAGIC,
    .priv_size       = sizeof(_priv_t)
};

/*** widget type: window ***/

#define _WIDGET_PRIV_TYPE	_priv_t
#define _WIDGET_CLASS_INF	_class_inf_window

static int _window_op_event(twtk_widget_t *widget, twtk_event_t *event, twtk_event_dispatch_t d)
{
    TWTK_WIDGET_OP_BEGIN

    if ((d == TWTK_EVENT_DISPATCH_DIRECT) &&
        (event->type == TWTK_EVENT_MOUSE) &&
	(event->mouse.event == TWTK_EVENT_MOUSE_MOVE))
    {
	if (event->mouse.all_buttons & TWTK_EVENT_MOUSE_BUTTON_MIDDLE)
	{
	    twtk_widget_move_rel(widget, event->mouse.diff);
	    TWTK_WIDGET_OP_RETURN(TWTK_EVSTAT_CONSUMED | TWTK_EVSTAT_DIRTY);
	}
    }

    if ((d == TWTK_EVENT_DISPATCH_DIRECT) &&
        (event->type == TWTK_EVENT_ACTION) &&
        (event->action.action == TWTK_EVENT_ACTION_PUSH))
    {
        _DEBUG("button action from: %s", event->action.widget->name);

        if (STRBUF_CMPSTR(event->action.widget->name, CLOSEBTN_NAME))
            _DEBUG("close buttton pressed");
    }

    TWTK_WIDGET_OP_END(0);
}

static int _window_op_insert_child(twtk_widget_t *widget, twtk_widget_t *child)
{
    TWTK_WIDGET_OP_BEGIN

    twtk_widget_t *subframe = twtk_widget_list_find_by_name(&widget->frames, SUBFRAME_NAME);
    assert(subframe);

    /** make it direct child of us **/
    twtk_widget_set_parent(child, widget);

    /** map it into our subframe instead of ourself **/
    twtk_platform_map_widget(child, subframe);

    TWTK_WIDGET_OP_END(0);
}


/*** widget type: subframe ***/
#undef _WIDGET_PRIV_TYPE
#undef _WIDGET_CLASS_INF
#define _WIDGET_PRIV_TYPE	_priv_t
#define _WIDGET_CLASS_INF	_class_inf_subframe

/*** misc ***/

/** calculate a window geometry from visble (client) area **/
static inline twtk_rect_t calc_window_geometry(twtk_rect_t rect)
{
    rect.pos.x  -= WINDOW_BORDER_LEFT;
    rect.pos.y  -= SUBFRAME_TOP;
    rect.size.x += WINDOW_BORDER_LEFT + WINDOW_BORDER_RIGHT;
    rect.size.y += SUBFRAME_TOP + WINDOW_BORDER_BOTTOM;
    return rect;
}

/** create a subframe w/ given geometry (relative to parent) **/
static twtk_widget_t *_create_subframe(void)
{
    twtk_widget_t *frame = twtk_widget_alloc(&_class_inf_subframe);
    twtk_widget_set_name(frame, SUBFRAME_NAME);
    twtk_widget_set_background_color(frame, SUBFRAME_BACKGROUND_COLOR);
    twtk_widget_set_border_color(frame, SUBFRAME_BORDER_COLOR);
    twtk_widget_set_border_width(frame, SUBFRAME_BORDER_SIZE);

    return frame;
}

static void do_resize(twtk_widget_t *window, twtk_vector_t size)
{
    _priv_t *priv = TWTK_WIDGET_PRIV(window, _priv_t*);
    assert(priv);

    /* resize the window itself */
    twtk_widget_resize(window, size);
    twtk_widget_vresize(window, size);

    /* resize the subframe */
    twtk_widget_set_viewport_coords(
        priv->client_area,
        WINDOW_BORDER_LEFT,
        SUBFRAME_TOP,
        size.x - WINDOW_BORDER_LEFT - WINDOW_BORDER_RIGHT,
        (size.y - SUBFRAME_TOP + WINDOW_BORDER_BOTTOM)/2,
        0
    );

    printf("==> client area: %0.f %0.f %0.f %0.f -- %0.f %0.f\n",
	priv->client_area->viewport.pos.x,
        priv->client_area->viewport.pos.y,
	priv->client_area->viewport.size.x,
        priv->client_area->viewport.size.y,
        size.x - WINDOW_BORDER_LEFT + WINDOW_BORDER_RIGHT,
        size.y - SUBFRAME_TOP + WINDOW_BORDER_BOTTOM
    );

    /* resize close button widget */
    if (priv->close_button)
    {
        twtk_widget_move_coords(
            priv->close_button,
            size.x - WINDOW_BORDER_RIGHT - priv->close_button->viewport.size.x,
            WINDOW_BORDER_TOP
        );
    }
}

static twtk_widget_t *_create(twtk_rect_t rect)
{
    twtk_widget_t *window = twtk_widget_alloc(&_class_inf_window);
    _priv_t *priv = TWTK_WIDGET_PRIV(window, _priv_t*);
    assert(priv);

    twtk_widget_set_background_color(window, WINDOW_BACKGROUND_COLOR);
    twtk_widget_set_border_color(window, WINDOW_BORDER_COLOR);
    twtk_widget_set_border_width(window, WINDOW_BORDER_SIZE);
    twtk_widget_move(window, rect.pos);

    /* insert the client area as subframe (not as child) */
    assert(priv->client_area = _create_subframe());
    twtk_widget_set_frame(priv->client_area, window);

    /* create the close button */
    assert(priv->close_button = twtk_button_widget_create(CLOSEBTN_IMAGE, twtk_rect_null(), "window-close"));
    twtk_widget_set_name(priv->close_button, CLOSEBTN_NAME);
    twtk_widget_set_frame(priv->close_button, window);
    twtk_widget_set_parent(priv->close_button, window);

    do_resize(window, rect.size);

    return window;
}

twtk_widget_t *twtk_window_widget_create(twtk_widget_t *child)
{
    assert(child);

    twtk_rect_t rect = calc_window_geometry(child->viewport);
    twtk_widget_t *window = _create(calc_window_geometry(child->viewport));

    child->viewport.pos = TWTK_VECTOR(0, 0);

    twtk_widget_add_child(window, child, (child->name ? child->name : "sub0"));

    return window;
}
