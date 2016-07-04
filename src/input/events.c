
#define _DEBUG_NAME	"events"

#define _GNU_SOURCE

#include <errno.h>
#include <stdio.h>
#include <memory.h>
#include <assert.h>
#include <twtk/events.h>
#include <twtk-private/debug.h>


struct evnotify {
    const char* name;
    twtk_event_handler_t handler;
    void *priv;
};

#define MAX_NOTIFY	10

struct evnotify notify[MAX_NOTIFY] = { 0 };

const char *twtk_event_type_name(twtk_event_type_t t)
{
    switch (t)
    {
	case TWTK_EVENT_MOUSE:			return "MOUSE";
	case TWTK_EVENT_KEY:			return "KEY";
    }
    return "???";
}

const char *twtk_event_mouse_type_name(twtk_event_mouse_type_t t)
{
    switch (t)
    {
	case 0:					return 0;
	case TWTK_EVENT_MOUSE_MOVE:		return "MOVE";
	case TWTK_EVENT_MOUSE_BTN_UP:		return "BUTTON-UP";
	case TWTK_EVENT_MOUSE_BTN_DOWN:		return "BUTTON-DOWN";
	case TWTK_EVENT_MOUSE_BTN_CLICK:	return "BUTTON-CICK";
	case TWTK_EVENT_MOUSE_BTN_DBLCLICK:	return "BUTTON-DBLCLICK";
    }
    return "???";
}

const char *twtk_event_mouse_button_name(twtk_event_mouse_button_t b)
{
    switch (b)
    {
	case TWTK_EVENT_MOUSE_BUTTON_LEFT:	return "LEFT";
	case TWTK_EVENT_MOUSE_BUTTON_MIDDLE:	return "MIDDLE";
	case TWTK_EVENT_MOUSE_BUTTON_RIGHT:	return "RIGHT";
	case 0:					return "";
    }
    return "???";
}

static void twtk_dump_event_mouse (twtk_event_t *ev)
{
    char buffer[4096];

    snprintf(buffer, sizeof(buffer), "mouse: %-15s X=%4f Y=%4f dX=%4f dY=%4f [%s%s%s]",
	twtk_event_mouse_type_name(ev->mouse.event),
	ev->mouse.pos.x,
	ev->mouse.pos.y,
	ev->mouse.diff.x,
	ev->mouse.diff.y,
	(ev->mouse.all_buttons & TWTK_EVENT_MOUSE_BUTTON_LEFT)   ? "L" : " ",
	(ev->mouse.all_buttons & TWTK_EVENT_MOUSE_BUTTON_MIDDLE) ? "M" : " ",
	(ev->mouse.all_buttons & TWTK_EVENT_MOUSE_BUTTON_RIGHT)  ? "M" : " ");

    switch (ev->mouse.event)
    {
	case TWTK_EVENT_MOUSE_MOVE:
	    _DEBUG("%s", buffer);
	break;
	case TWTK_EVENT_MOUSE_BTN_DOWN:
	case TWTK_EVENT_MOUSE_BTN_UP:
	case TWTK_EVENT_MOUSE_BTN_CLICK:
	case TWTK_EVENT_MOUSE_BTN_DBLCLICK:
	    _DEBUG("%s %s", buffer, twtk_event_mouse_button_name(ev->mouse.button));
	break;
	default:
	    _DEBUG("%s", buffer);
    }
}

int twtk_event_dump (twtk_event_t *ev)
{
    assert(ev);
    switch (ev->type)
    {
	case TWTK_EVENT_MOUSE:
	    twtk_dump_event_mouse(ev);
	break;
	default:
	    _DEBUG("twtk: unknown event type: %d\n", ev->type);
	break;
    }
    return 1;
}

int twtk_event_dump_handler (twtk_event_t *ev, void *priv)
{
    return twtk_event_dump(ev);
}

int twtk_event_raise (twtk_event_t *ev)
{
    int x=0;
    for (x=0; x<MAX_NOTIFY; x++)
	if (notify[x].handler)
	    notify[x].handler(ev, notify[x].priv);
    return 1;
}

int twtk_event_notify(const char *name, twtk_event_handler_t handler, void *priv)
{
    int x;
    for (x=0; x<MAX_NOTIFY; x++)
    {
	if (notify[x].handler != NULL)
	    continue;

	notify[x].name = strdup ( name ? name : "<anonymous>" );
	notify[x].handler = handler;
	notify[x].priv = priv;
	return 0;
    }

    return -EMLINK;
}
