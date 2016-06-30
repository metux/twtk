
#ifdef ENABLE_DEBUG_PLATFORM
#define _GNU_SOURCE
#endif

#include <assert.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <twtk/platform.h>
#include <twtk-private/drm.h>
#include <twtk-private/xcb.h>
#include <twtk/events.h>
#include <twtk/widget.h>
#include <twtk/evdev.h>
#include <twtk-private/debug-widget.h>


twtk_platform_t *_twtk_current_platform = NULL;

void twtk_platform_install(twtk_platform_t *platform)
{
    assert(!_twtk_current_platform);
    assert(platform);
    _twtk_current_platform = platform;
}

#ifdef ENABLE_PLATFORM_DRM
static inline int _setup_drm()
{
    const char *mouse_device = getenv("TWTK_MOUSE_DEVICE");

    twtk_platform_install(twtk_platform_drm_init());
    twtk_evdev_mouse_start(mouse_device);

    return 0;
}
#endif

#ifdef ENABLE_PLATFORM_XCB
static inline int _setup_xcb()
{
    if (!getenv("DISPLAY"))
        return -ENOENT;

    twtk_platform_t *xcb_platform = twtk_platform_xcb_init();

    if (xcb_platform == NULL)
        return -EPERM;

    twtk_platform_install(xcb_platform);

    return 0;
}
#endif

void twtk_platform_setup()
{
    twtk_platform_init_dispatch();

#ifdef ENABLE_PLATFORM_XCB
    if (!_setup_xcb())
        return;
#endif

#ifdef ENABLE_PLATFORM_DRM
    if (!_setup_drm())
        return;
#endif
}

void twtk_platform_finish()
{
    if (!_twtk_current_platform)
	return;

    _twtk_current_platform->op_destroy(_twtk_current_platform);
}

// FIXME: this only works if the widget is root or 1st level
static void __twtk_platform_redraw_widget(twtk_widget_t *widget, const char *name)
{
    assert(widget);
    assert(_twtk_current_platform);
    cairo_t *cr = _twtk_current_platform->op_get_context(_twtk_current_platform);
    assert(cr);
    twtk_widget_do_draw(widget, cr);
    _twtk_current_platform->op_free_context(_twtk_current_platform, cr);
}

void twtk_platform_redraw()
{
    twtk_widget_t *root = _twtk_current_platform->op_get_root(_twtk_current_platform);
    if (!root)
	return;

    __twtk_platform_redraw_widget(root, "<root>");
}

// FIXME: for now we'll have to do a full repaint, until __twtk_platform_redraw_widget() is fixed
void twtk_platform_redraw_widget(twtk_widget_t *widget, const char *name)
{
    twtk_platform_redraw();
}

void twtk_platform_loop()
{
    if (!_twtk_current_platform)
        return;

    assert(_twtk_current_platform->op_mainloop);
    _twtk_current_platform->op_mainloop(_twtk_current_platform);
}

void twtk_platform_stop()
{
    if (!_twtk_current_platform)
        return;

    assert(_twtk_current_platform->op_stop);
    _twtk_current_platform->op_stop(_twtk_current_platform);
}

static inline int _is_dbg_event(twtk_event_t *event)
{
    return ((event->type == TWTK_EVENT_MOUSE) &&
	((event->mouse.event == TWTK_EVENT_MOUSE_BTN_DOWN) ||
	 (event->mouse.event == TWTK_EVENT_MOUSE_BTN_UP)));
}

static int _dispatch_handler(twtk_event_t *event, void *priv)
{
    /* currently only supporting mouse events */
    if (event->type == TWTK_EVENT_MOUSE)
    {
	if (_is_dbg_event(event))
	{
	    char buffer[1024];
	    snprintf(buffer, sizeof(buffer), "[ %4.0f : %4.0f ] <ROOT>",
		event->mouse.pos.x,
		event->mouse.pos.y);

	    twtk_debug_widget_set_text(buffer);
	}

	int ret = twtk_widget_dispatch(
	    _twtk_current_platform->op_get_root(_twtk_current_platform),
	    event);

	if (ret & TWTK_EVSTAT_DIRTY)
	{
	    /* we need to redraw */
	    // FIXME: redraw'ing the whole screen isn't actually efficient
	    twtk_platform_redraw();
	}

	if (_is_dbg_event(event))
	    twtk_debug_widget_redraw();
    }

    return 0;
}

void twtk_platform_init_dispatch()
{
    twtk_event_notify("dispatch", _dispatch_handler, NULL);
}

int twtk_platform_map_widget(twtk_widget_t *widget, twtk_widget_t *parent)
{
    assert(widget);
    assert(_twtk_current_platform);
    assert(_twtk_current_platform->op_map_widget);

    return _twtk_current_platform->op_map_widget(
        _twtk_current_platform, widget, parent);
}
