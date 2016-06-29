
#define _DEBUG_NAME	"mtdev"

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <malloc.h>
#include <pthread.h>
#include <libevdev-1.0/libevdev/libevdev.h>
#include <twtk/evdev.h>
#include <twtk/events.h>
#include <twtk-private/debug.h>


#define SYM_MAP_BEGIN \
    static char buffer[16]; \
    switch (code) \
    {

#define SYM_MAP_FINI(name) \
        default: \
            snprintf(buffer, sizeof(buffer), "(" name "#%d)", code); \
            return buffer; \
    }

#define SYM_MAP_ENT(sym) \
    case sym: return #sym;


const char* twtk_mtdev_get_event_type_name(uint16_t code)
{
    SYM_MAP_BEGIN
    SYM_MAP_ENT(EV_SYN);
    SYM_MAP_ENT(EV_KEY);
    SYM_MAP_ENT(EV_REL);
    SYM_MAP_ENT(EV_ABS);
    SYM_MAP_ENT(EV_MSC);
    SYM_MAP_ENT(EV_SW);
    SYM_MAP_ENT(EV_LED);
    SYM_MAP_ENT(EV_SND);
    SYM_MAP_ENT(EV_REP);
    SYM_MAP_ENT(EV_FF);
    SYM_MAP_ENT(EV_PWR);
    SYM_MAP_ENT(EV_FF_STATUS);
    SYM_MAP_FINI("EV");
}

static const char *_ev_code_syn(uint16_t code)
{
    SYM_MAP_BEGIN
    SYM_MAP_ENT(SYN_REPORT);
    SYM_MAP_ENT(SYN_CONFIG);
    SYM_MAP_ENT(SYN_MT_REPORT);
    SYM_MAP_ENT(SYN_DROPPED);
    SYM_MAP_FINI("SYN")
}

static const char *_ev_code_key(uint16_t code)
{
    SYM_MAP_BEGIN
    SYM_MAP_ENT(BTN_TOOL_PEN);
    SYM_MAP_ENT(BTN_TOOL_RUBBER);
    SYM_MAP_ENT(BTN_TOOL_BRUSH);
    SYM_MAP_ENT(BTN_TOOL_PENCIL);
    SYM_MAP_ENT(BTN_TOOL_AIRBRUSH);
    SYM_MAP_ENT(BTN_TOOL_FINGER);
    SYM_MAP_ENT(BTN_TOOL_MOUSE);
    SYM_MAP_ENT(BTN_TOOL_LENS);
    SYM_MAP_ENT(BTN_TOOL_QUINTTAP);
    SYM_MAP_ENT(BTN_TOOL_DOUBLETAP);
    SYM_MAP_ENT(BTN_TOOL_TRIPLETAP);
    SYM_MAP_ENT(BTN_TOOL_QUADTAP);
    SYM_MAP_ENT(BTN_TOUCH);
    SYM_MAP_ENT(BTN_LEFT);
    SYM_MAP_ENT(BTN_RIGHT);
    SYM_MAP_ENT(BTN_MIDDLE);
    SYM_MAP_ENT(BTN_SIDE);
    SYM_MAP_ENT(BTN_EXTRA);
    SYM_MAP_ENT(BTN_FORWARD);
    SYM_MAP_ENT(BTN_BACK);
    SYM_MAP_ENT(BTN_TASK);
    SYM_MAP_FINI("BTN")
}

static const char *_ev_code_rel(uint16_t code)
{
    SYM_MAP_BEGIN
    SYM_MAP_ENT(REL_X);
    SYM_MAP_ENT(REL_Y);
    SYM_MAP_ENT(REL_Z);
    SYM_MAP_ENT(REL_RX);
    SYM_MAP_ENT(REL_RY);
    SYM_MAP_ENT(REL_RZ);
    SYM_MAP_ENT(REL_HWHEEL);
    SYM_MAP_ENT(REL_DIAL);
    SYM_MAP_ENT(REL_WHEEL);
    SYM_MAP_ENT(REL_MISC);
    SYM_MAP_FINI("REL");
}

static const char *_ev_code_abs(uint16_t code)
{
    SYM_MAP_BEGIN
    SYM_MAP_ENT(ABS_DISTANCE);
    SYM_MAP_ENT(ABS_X);
    SYM_MAP_ENT(ABS_Y);
    SYM_MAP_ENT(ABS_Z);
    SYM_MAP_ENT(ABS_PRESSURE);
    SYM_MAP_ENT(ABS_MT_SLOT);
    SYM_MAP_ENT(ABS_MT_TOUCH_MAJOR);
    SYM_MAP_ENT(ABS_MT_TOUCH_MINOR);
    SYM_MAP_ENT(ABS_MT_WIDTH_MAJOR);
    SYM_MAP_ENT(ABS_MT_WIDTH_MINOR);
    SYM_MAP_ENT(ABS_MT_ORIENTATION);
    SYM_MAP_ENT(ABS_MT_POSITION_X);
    SYM_MAP_ENT(ABS_MT_POSITION_Y);
    SYM_MAP_ENT(ABS_MT_TOOL_TYPE);
    SYM_MAP_ENT(ABS_MT_BLOB_ID);
    SYM_MAP_ENT(ABS_MT_TRACKING_ID);
    SYM_MAP_ENT(ABS_MT_PRESSURE);
    SYM_MAP_ENT(ABS_MT_DISTANCE);
    SYM_MAP_ENT(ABS_MT_TOOL_X);
    SYM_MAP_ENT(ABS_MT_TOOL_Y);
    SYM_MAP_ENT(ABS_RX);
    SYM_MAP_ENT(ABS_RY);
    SYM_MAP_ENT(ABS_RZ);
    SYM_MAP_ENT(ABS_HAT0X);
    SYM_MAP_ENT(ABS_HAT0Y);
    SYM_MAP_ENT(ABS_HAT1X);
    SYM_MAP_ENT(ABS_HAT1Y);
    SYM_MAP_ENT(ABS_HAT2X);
    SYM_MAP_ENT(ABS_HAT2Y);
    SYM_MAP_ENT(ABS_TILT_X);
    SYM_MAP_ENT(ABS_TILT_Y);
    SYM_MAP_ENT(ABS_TOOL_WIDTH);
    SYM_MAP_FINI("ABS");
}

static const char *_ev_code_msc(uint16_t code)
{
    SYM_MAP_BEGIN
    SYM_MAP_ENT(MSC_TIMESTAMP);
    SYM_MAP_ENT(MSC_PULSELED);
    SYM_MAP_ENT(MSC_SERIAL);
    SYM_MAP_ENT(MSC_GESTURE);
    SYM_MAP_ENT(MSC_RAW);
    SYM_MAP_ENT(MSC_SCAN);
    SYM_MAP_ENT(MSC_MAX);
    SYM_MAP_ENT(MSC_CNT);
    SYM_MAP_FINI("MSC");
}

static const char *_ev_code_sw(uint16_t code)
{
    SYM_MAP_BEGIN
    SYM_MAP_ENT(SW_LID);
    SYM_MAP_FINI("SW");
}

static const char *_ev_code_led(uint16_t code)
{
    SYM_MAP_BEGIN
    SYM_MAP_FINI("LED");
}

static const char *_ev_code_snd(uint16_t code)
{
    SYM_MAP_BEGIN
    SYM_MAP_FINI("SND");
}

static const char *_ev_code_rep(uint16_t code)
{
    SYM_MAP_BEGIN
    SYM_MAP_FINI("REP");
}

static const char *_ev_code_ff(uint16_t code)
{
    SYM_MAP_BEGIN
    SYM_MAP_FINI("FF");
}

static const char *_ev_code_pwr(uint16_t code)
{
    SYM_MAP_BEGIN
    SYM_MAP_FINI("PWR");
}

static const char *_ev_code_ff_status(uint16_t code)
{
    SYM_MAP_BEGIN
    SYM_MAP_FINI("FF_STATUS");
}

const char* twtk_mtdev_get_event_code_name(uint16_t type, uint16_t code)
{
    static char buffer[64];
    switch (type)
    {
	case EV_SYN:		return _ev_code_syn(code);
	case EV_KEY:		return _ev_code_key(code);
	case EV_REL:		return _ev_code_rel(code);
	case EV_ABS:		return _ev_code_abs(code);
	case EV_MSC:		return _ev_code_msc(code);
	case EV_SW:		return _ev_code_sw(code);
	case EV_LED:		return _ev_code_led(code);
	case EV_SND:		return _ev_code_snd(code);
	case EV_REP:		return _ev_code_rep(code);
	case EV_FF:		return _ev_code_ff(code);
	case EV_PWR:		return _ev_code_pwr(code);
	case EV_FF_STATUS:	return _ev_code_ff_status(code);
	default:
	    snprintf(buffer, sizeof(buffer), "??? #%2x #%2x", type, code);
	    return buffer;
    }
}

struct mtdev_state
{
    pthread_t			thread;
    struct mtdev		mtdev;
    twtk_vector_t		last_position;
    twtk_event_mouse_button_t	last_buttons;
    int				fd;
    int				id;
    struct mtdev		*dev;
};

static inline twtk_event_mouse_button_t evcode_to_button(uint16_t code)
{
    switch (code)
    {
	case BTN_LEFT:		return TWTK_EVENT_MOUSE_BUTTON_LEFT;
	case BTN_MIDDLE:	return TWTK_EVENT_MOUSE_BUTTON_MIDDLE;
	case BTN_RIGHT:		return TWTK_EVENT_MOUSE_BUTTON_RIGHT;
	default:		return 0;
    }
}

static int _handle_mouse_event(struct mtdev_state *state, struct input_event *ev)
{
    twtk_event_t event = {
	.type = TWTK_EVENT_MOUSE,
	.device = state->id
    };

    switch (ev->type)
    {
	case EV_REL:
	{
	    switch (ev->code)
	    {
		case REL_X:
		    state->last_position.x += ev->value;
		    event.mouse.relative.x = ev->value;
		break;
		case REL_Y:
		    state->last_position.y += ev->value;
		    event.mouse.relative.y = ev->value;
		break;
		default:
		    return 0;
	    }
	    event.mouse.event = TWTK_EVENT_MOUSE_MOVE;
	    event.mouse.absolute = state->last_position;
	    event.mouse.all_buttons = state->last_buttons;
	    return twtk_event_raise(&event);
	}

        /* translate absolute touchpad events to relative mouse events */
	case EV_ABS:
	{
	    switch (ev->code)
	    {
		case ABS_X:
		    event.mouse.relative.x = ev->value - state->last_position.x;
		    state->last_position.x = ev->value;
		    _DEBUG("ABS_X: %d", ev->value);
		break;
		case ABS_Y:
		    event.mouse.relative.y = ev->value - state->last_position.y;
		    state->last_position.y = ev->value;
		    _DEBUG("ABS_Y: %d", ev->value);
		break;
		default:
		    return 0;
	    }
	    event.mouse.event = TWTK_EVENT_MOUSE_MOVE;
	    event.mouse.absolute = state->last_position;
	    event.mouse.all_buttons = state->last_buttons;
	    return twtk_event_raise(&event);
	}

	case EV_KEY:
	{
	    event.mouse.button = evcode_to_button(ev->code);
	    event.mouse.absolute = state->last_position;

	    if (ev->value)
	    {
		state->last_buttons |= event.mouse.button;
		event.mouse.event = TWTK_EVENT_MOUSE_BTN_DOWN;
		_DEBUG("KEY DN: %d at %f:%f", event.mouse.button, event.mouse.absolute.x, event.mouse.absolute.y);
	    }
	    else
	    {
		state->last_buttons &= ~event.mouse.button;
		event.mouse.event = TWTK_EVENT_MOUSE_BTN_UP;
		_DEBUG("KEY UP: %d at %f:%f", event.mouse.button, event.mouse.absolute.x, event.mouse.absolute.y);
	    }

	    event.mouse.all_buttons = state->last_buttons;
	    return twtk_event_raise(&event);
	}

	case EV_SYN:
	    _DEBUG("SYN");
	    return 1;	/* ignore it */

	case EV_MSC:
	{
	    switch (ev->code)
	    {
		case MSC_SCAN:
		{
		    switch (ev->value)
		    {
			/* ugly hack ... just ignore known irrelevant scan codes for now */
			case 589825:
			case 589826:
			case 589827:
			    return 1;
		    }
		}
	    }
	}

	default:
	    return 0;
    }

    return 0;
}

void twtk_mtdev_mouse_loop(struct mtdev_state* state)
{
    int rc = 0;
    do {
	struct input_event ev;
	if ((rc = libmtdev_next_event(state->dev, LIBMTDEV_READ_FLAG_NORMAL, &ev)) == 0)
	{
	    if (!_handle_mouse_event(state, &ev))
	    {
		_DEBUG("Event: %-10s %-10s %5d",
		    twtk_mtdev_get_event_type_name(ev.type),
		    twtk_mtdev_get_event_code_name(ev.type, ev.code),
		    ev.value);
	    }
	}
    } while (rc == 1 || rc == 0 || rc == -EAGAIN);
    free(state);
}

static void *_mouse_thread(void *ptr)
{
    twtk_mtdev_mouse_loop((struct mtdev_state*)ptr);
}

void twtk_mtdev_mouse_start(const char* devname)
{
    _DEBUG("Input device: %s", devname);

    struct mtdev_state *state = calloc(1, sizeof(struct mtdev_state));

    static last_device_id = 0;

    state->id = ++last_device_id;

    if ((state->fd = open(devname, O_RDONLY | O_NONBLOCK)) == -1) {
	_DEBUG("failed opening event device: %s", strerror(errno));
	return;
    }

    int rc = mtdev_open (&state->dev, state->fd);
    if (rc)
    {
	_DEBUG("failed opening mtdev: %s", strerror(-rc));
	return;
    }

    int res = pthread_create(&state->thread, NULL, _mouse_thread, state);
    if (!res)
	_DEBUG("thread creation failed: %s", strerror(errno));
}
