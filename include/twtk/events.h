#ifndef __TWTK_EVENTS_H
#define __TWTK_EVENTS_H

#include <twtk/types.h>
#include <twtk/vector.h>

typedef enum
{
    TWTK_EVENT_MOUSE	= 1,
    TWTK_EVENT_KEY	= 2,
} twtk_event_type_t;


typedef enum
{
    TWTK_EVENT_MOUSE_LEAVE		= 0,
    TWTK_EVENT_MOUSE_MOVE		= 1,
    TWTK_EVENT_MOUSE_BTN_UP		= 2,
    TWTK_EVENT_MOUSE_BTN_DOWN		= 4,
    TWTK_EVENT_MOUSE_BTN_CLICK		= 8,
    TWTK_EVENT_MOUSE_BTN_DBLCLICK	= 16
} twtk_event_mouse_type_t;


typedef enum
{
    TWTK_EVENT_MOUSE_BUTTON_LEFT	= 1,
    TWTK_EVENT_MOUSE_BUTTON_MIDDLE	= 2,
    TWTK_EVENT_MOUSE_BUTTON_RIGHT	= 4
} twtk_event_mouse_button_t;


struct __twtk_event
{
    int device;
    twtk_event_type_t type;
    union {
	struct {
	    twtk_event_mouse_type_t event;
	    twtk_vector_t pos;
	    twtk_vector_t diff;
	    twtk_event_mouse_button_t all_buttons;
	    twtk_event_mouse_button_t button;
	} mouse;
    };
};

typedef int (*twtk_event_handler_t)(twtk_event_t *event, void *priv);

const char *twtk_event_type_name(twtk_event_type_t t);
const char *twtk_event_mouse_type_name(twtk_event_mouse_type_t t);
const char *twtk_event_mouse_button_name(twtk_event_mouse_button_t b);

int twtk_event_notify(const char* name, twtk_event_handler_t handler, void *priv);
int twtk_event_raise(twtk_event_t *ev);
int twtk_event_dump(twtk_event_t *ev);

int twtk_event_dump_handler(twtk_event_t *ev, void *priv);

#endif /* __TWTK_EVENTS_H */
