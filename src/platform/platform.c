
#define _GNU_SOURCE

#include <assert.h>
#include <stdio.h>
#include <unistd.h>
#include <twtk/platform.h>
#include <twtk-private/drm.h>
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

void twtk_platform_setup()
{
    const char *mouse_device = getenv("TWTK_MOUSE_DEVICE");

    twtk_platform_install(twtk_platform_drm_init());
    twtk_evdev_mouse_start(mouse_device);
    twtk_platform_init_dispatch();
}

void twtk_platform_finish()
{
    if (!_twtk_current_platform)
	return;

    _twtk_current_platform->op_destroy(_twtk_current_platform);
}

// FIXME: this only works if the widget is root or 1st level
void twtk_platform_redraw_widget(twtk_widget_t *widget, const char *name)
{
    assert(widget);
    assert(_twtk_current_platform);
    cairo_t *cr = _twtk_current_platform->op_get_context(_twtk_current_platform);
    assert(cr);
    twtk_widget_do_draw(widget,cr, name);
    _twtk_current_platform->op_free_context(_twtk_current_platform, cr);
}

void twtk_platform_redraw()
{
    twtk_widget_t *root = _twtk_current_platform->op_get_root(_twtk_current_platform);
    if (!root)
	return;

    twtk_platform_redraw_widget(root, "<root>");
}

void twtk_platform_loop()
{
    while (1)
    {
	usleep(1000000);
    }
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
		event->mouse.absolute.x,
		event->mouse.absolute.y);

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
