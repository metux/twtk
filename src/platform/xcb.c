
#define _DEBUG_NAME	"xcb"

#ifdef ENABLE_DEBUG_PLATFORM_XCB
#define ENABLE_DEBUG
#endif

#include <unistd.h>
#include <stdint.h>
#include <inttypes.h>
#include <stdio.h>
#include <sys/mman.h>
#include <string.h>
#include <errno.h>
#include <malloc.h>
#include <assert.h>
#include <xcb/xcb.h>
#include <cairo-xcb.h>
#include <stdio.h>
#include <stdlib.h>
#include <twtk/events.h>
#include <twtk/widget.h>
#include <twtk/color.h>
#include <twtk/platform.h>
#include <twtk-private/xcb.h>
#include <twtk-private/xcb-util.h>
#include <twtk-private/debug.h>
#include <twtk-private/platform-generic.h>


typedef struct
{
    twtk_platform_t base;
    twtk_widget_t *root;
    xcb_connection_t *conn;
    xcb_screen_t *screen;
    xcb_window_t window;
    xcb_visualtype_t *visual;
    int all_buttons;
    twtk_vector_t last_pos;
    int running;
} twtk_platform_xcb_t;


static int _xcb_destroy(twtk_platform_t *platform)
{
    assert(platform);

    twtk_platform_xcb_t *xcb_platform = (twtk_platform_xcb_t*) platform;

    // FIXME: should free the cursor
    cairo_surface_finish(platform->surface);
    cairo_surface_destroy(platform->surface);
    xcb_disconnect(xcb_platform->conn);

    free(platform);
    return 0;
}

static int _xcb_handle_expose(
    twtk_platform_xcb_t *platform,
    xcb_expose_event_t *event)
{
    /* Avoid extra redraws by checking if this is
     * the last expose event in the sequence
     */
    if (event->count != 0)
        return 0;

    twtk_platform_redraw();
}

static int _xcb_handle_configure_notify(
    twtk_platform_xcb_t *platform,
    xcb_configure_notify_event_t *event)
{
    if (event->window != platform->window)
    {
        _DEBUG(" -- window mismatch");
        return 0;
    }

    twtk_widget_t *root = platform->base.op_get_root(&platform->base);
    assert(root);

    /** note: for now we leave viewport.pos at (0,0), otherwise we'll
        end up with having an offset *within* the window

        for multi-window mode, we'll have to rethink the rendering
        and position handling
    **/

    if ((root->viewport.size.x != event->width) ||
        (root->viewport.size.y != event->height))
    {
        _DEBUG("window resized from %.0f x %.0f to %d x %d",
            root->viewport.size.x,
            root->viewport.size.y,
            event->width,
            event->height);
        cairo_xcb_surface_set_size(platform->base.surface, event->width, event->height);
        root->viewport.size = TWTK_VECTOR(event->width, event->height);
    }

    twtk_platform_redraw();
}

static int _xcb_to_twtk_button(xcb_button_t btn)
{
    switch (btn)
    {
        case 1: return TWTK_EVENT_MOUSE_BUTTON_LEFT;
        case 2: return TWTK_EVENT_MOUSE_BUTTON_MIDDLE;
        case 3: return TWTK_EVENT_MOUSE_BUTTON_RIGHT;
        default: return 0;
    }
    return 0;
}

static int _xcb_ev_mouse_pos(twtk_platform_xcb_t *pl, twtk_event_t *ev, int x, int y)
{
    ev->mouse.diff = TWTK_VECTOR(
        ev->mouse.pos.x = x - pl->last_pos.x,
        ev->mouse.pos.y = y - pl->last_pos.y
    );

    ev->mouse.pos = pl->last_pos = TWTK_VECTOR(x, y);

    return 0;
}

static int _xcb_handle_button_release(twtk_platform_xcb_t *pl, xcb_button_release_event_t *ev)
{
    if (pl->window != ev->event)
    {
        _DEBUG("wrong window ? my=%d event=%d", pl->window, ev->event);
        return 0;
    }

    int btn = _xcb_to_twtk_button(ev->detail);

    pl->all_buttons &= ~ btn;

    twtk_event_t event = (twtk_event_t){
        .type  = TWTK_EVENT_MOUSE,
        .mouse = {
            .event       = TWTK_EVENT_MOUSE_BTN_UP,
            .button      = btn,
            .all_buttons = pl->all_buttons,
        }
    };

    _xcb_ev_mouse_pos(pl, &event, ev->event_x, ev->event_y);

    twtk_event_raise(&event);

    return 0;
}

static int _xcb_handle_button_press(twtk_platform_xcb_t *pl, xcb_button_press_event_t *ev)
{
    if (pl->window != ev->event)
    {
        _DEBUG("wrong window ? my=%d event=%d", pl->window, ev->event);
        return 0;
    }

    int btn = _xcb_to_twtk_button(ev->detail);

    pl->all_buttons |= btn;

    twtk_event_t event = (twtk_event_t){
        .type  = TWTK_EVENT_MOUSE,
        .mouse = {
            .event       = TWTK_EVENT_MOUSE_BTN_DOWN,
            .button      = btn,
            .all_buttons = pl->all_buttons,
        }
    };

    _xcb_ev_mouse_pos(pl, &event, ev->event_x, ev->event_y);

    twtk_event_raise(&event);

    return 0;
}

static int _xcb_handle_motion_notify(twtk_platform_xcb_t *pl, xcb_motion_notify_event_t* ev)
{
    if (ev->event != pl->window)
    {
        _DEBUG("motion: wrong window");
        return -ENOENT;
    }

    twtk_event_t event = (twtk_event_t){
        .type  = TWTK_EVENT_MOUSE,
        .mouse = {
            .event = TWTK_EVENT_MOUSE_MOVE,
            .all_buttons = pl->all_buttons
        }
    };

    _xcb_ev_mouse_pos(pl, &event, ev->event_x, ev->event_y);

    twtk_event_raise(&event);

    return 0;
}

static int _xcb_handle_event(twtk_platform_xcb_t *pl, xcb_generic_event_t *ev)
{
    assert(pl);
    assert(ev);

    switch (ev->response_type & ~0x80)
    {
        case XCB_EXPOSE:
            return _xcb_handle_expose(pl, (xcb_expose_event_t*)ev);

        case XCB_CONFIGURE_NOTIFY:
            return _xcb_handle_configure_notify(pl, (xcb_configure_notify_event_t*)ev);

        case XCB_BUTTON_PRESS:
            return _xcb_handle_button_press(pl, (xcb_button_press_event_t*)ev);

        case XCB_BUTTON_RELEASE:
            return _xcb_handle_button_release(pl, (xcb_button_release_event_t*)ev);

        case XCB_MOTION_NOTIFY:
            return _xcb_handle_motion_notify(pl, (xcb_motion_notify_event_t*)ev);

        case XCB_KEY_PRESS:
            return 0;

        case XCB_KEY_RELEASE:
            return 0;

        case XCB_ENTER_NOTIFY:
            return 0;

        case XCB_LEAVE_NOTIFY:
            return 0;

        case XCB_FOCUS_IN:
            return 0;

        case XCB_FOCUS_OUT:
            return 0;

        default:
            _DEBUG("event id %d", ev->response_type);
        break;
    }
}

static int _xcb_mainloop(twtk_platform_t *platform)
{
    assert(platform);
    twtk_platform_xcb_t *xcb_platform = (twtk_platform_xcb_t*) platform;

    cairo_t *cr = cairo_create(platform->surface);

    xcb_flush(xcb_platform->conn);

    xcb_platform->running = 1;

    xcb_generic_event_t *event;
    while ((xcb_platform->running) && (event = xcb_wait_for_event(xcb_platform->conn)))
    {
        _xcb_handle_event(xcb_platform, event);
        free(event);
        xcb_flush(xcb_platform->conn);
    }

    return 0;
}

static int _xcb_stop(twtk_platform_t *platform)
{
    assert(platform);
    twtk_platform_xcb_t *xcb_platform = (twtk_platform_xcb_t*) platform;

    xcb_platform->running = 0;

    return 0;
}

static int _connect(twtk_platform_xcb_t *xcb_platform)
{
    xcb_platform->conn = xcb_connect(NULL, NULL);

    if (xcb_connection_has_error(xcb_platform->conn)) {
        _DEBUG("Could not connect to X11 server");
        return -ENOENT;
    }

    xcb_platform->screen = xcb_setup_roots_iterator(xcb_get_setup(xcb_platform->conn)).data;
    xcb_platform->visual = xcbutil_find_visual(xcb_platform->conn, xcb_platform->screen->root_visual);
    if (xcb_platform->visual == NULL) {
        _DEBUG("Some weird internal error...?!");
        xcb_disconnect(xcb_platform->conn);
        return -ENOENT;
    }

    return 0;
}

/** Set an atom with string value **/
static int _xcb_set_atom_str(xcb_connection_t *conn, xcb_window_t window, xcb_atom_t atom, const char *value)
{
    xcb_change_property (conn,
                         XCB_PROP_MODE_REPLACE,
                         window,
                         XCB_ATOM_WM_NAME,
                         XCB_ATOM_STRING,
                         8,
                         strlen (value),
                         value );
}

/** Set an atom with atom value **/
static int _xcb_set_atom_atom(xcb_connection_t *conn, xcb_window_t window, xcb_atom_t atom, xcb_atom_t value)
{
    xcb_change_property (conn,
                         XCB_PROP_MODE_REPLACE,
                         window,
                         XCB_ATOM_WM_NAME,
                         XCB_ATOM_ATOM,
                         32,
                         1,
                         &value);
}

static int _xcb_set_atom_n_enum(xcb_connection_t *conn, xcb_window_t window, const char* name, const char *value)
{
    return _xcb_set_atom_atom(
        conn,
        window,
        xcbutil_get_atom(conn, name),
        xcbutil_get_atom(conn, value));
}

static int _create_window(
    twtk_platform_xcb_t *xcb_platform,
    xcb_rectangle_t geometry,
    xcb_window_t *r_window
)
{
    assert(xcb_platform);
    assert(r_window);

    uint32_t mask[1] = {
        XCB_EVENT_MASK_EXPOSURE         |
        XCB_EVENT_MASK_BUTTON_PRESS     |
        XCB_EVENT_MASK_BUTTON_RELEASE   |
        XCB_EVENT_MASK_POINTER_MOTION   |
        XCB_EVENT_MASK_BUTTON_MOTION    |
        XCB_EVENT_MASK_BUTTON_1_MOTION  |
        XCB_EVENT_MASK_BUTTON_2_MOTION  |
        XCB_EVENT_MASK_BUTTON_3_MOTION  |
        XCB_EVENT_MASK_BUTTON_4_MOTION  |
        XCB_EVENT_MASK_BUTTON_5_MOTION  |
        XCB_EVENT_MASK_ENTER_WINDOW     |
        XCB_EVENT_MASK_LEAVE_WINDOW     |
        XCB_EVENT_MASK_KEY_PRESS        |
        XCB_EVENT_MASK_KEY_RELEASE      |
        XCB_EVENT_MASK_STRUCTURE_NOTIFY
    };

    xcb_window_t window = xcb_generate_id(xcb_platform->conn);
    xcb_create_window(
        xcb_platform->conn,
        XCB_COPY_FROM_PARENT,
        window,
        xcb_platform->screen->root,
        geometry.x,
        geometry.y,
        geometry.width,
        geometry.height,
        0,					/* border */
        XCB_WINDOW_CLASS_INPUT_OUTPUT,
        xcb_platform->screen->root_visual,
        XCB_CW_EVENT_MASK,
        mask);

    xcbutil_set_window_title(xcb_platform->conn, window, "Hello world");

    xcb_map_window(xcb_platform->conn, window);

    xcb_flush (xcb_platform->conn);

    *r_window = window;

    return 0;
}

twtk_platform_t *twtk_platform_xcb_init()
{
    TWTK_NEW_OBJECT(twtk_platform_xcb_t,xcb_platform);

    int ret = _connect(xcb_platform);

    assert(ret == 0);

    _twtk_platform_generic_init(&xcb_platform->base);

    xcb_rectangle_t rect;
    xcbutil_get_workspace_area(xcb_platform->conn, xcb_platform->screen->root, 0, &rect);

    ret = _create_window(xcb_platform, rect, &xcb_platform->window);
    assert(ret == 0);

    if (!(xcb_platform->base.surface = cairo_xcb_surface_create(
            xcb_platform->conn,
            xcb_platform->window,
            xcb_platform->visual,
            rect.width,
            rect.height)))
    {
        _DEBUG("failed to create xcb service");
        return NULL;
    }

    xcb_platform->base.op_mainloop     = _xcb_mainloop;
    xcb_platform->base.op_stop         = _xcb_stop;
    xcb_platform->base.op_destroy      = _xcb_destroy;

    return &xcb_platform->base;
}
