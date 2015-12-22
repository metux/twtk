
#define _DEBUG_NAME	"evdev"

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

const char* twtk_evdev_get_event_type_name(uint16_t type)
{
    switch (type)
    {
	case EV_SYN:		return "EV_SYN";
	case EV_KEY:		return "EV_KEY";
	case EV_REL:		return "EV_REL";
	case EV_ABS:		return "EV_ABS";
	case EV_MSC:		return "EV_MSC";
	case EV_SW:		return "EV_SW";
	case EV_LED:		return "EV_LED";
	case EV_SND:		return "EV_SND";
	case EV_REP:		return "EV_REP";
	case EV_FF:		return "EV_FF";
	case EV_PWR:		return "EV_PWR";
	case EV_FF_STATUS:	return "EV_FF_STATUS";
	default:		return "???";
    }
}

static const char *_ev_code_syn(uint16_t code)
{
    switch (code)
    {
	case SYN_REPORT:	return "SYN_REPORT";
	case SYN_CONFIG:	return "SYN_CONFIG";
	case SYN_MT_REPORT:	return "SYN_MT_REPORT";
	case SYN_DROPPED:	return "SYN_DROPPED";
	default:		return "???";
    }
}

static const char *_ev_code_key(uint16_t code)
{
    static char buffer[64];
    switch (code)
    {
	case BTN_TOOL_PEN:		return "BTN_TOOL_PEN";
	case BTN_TOOL_RUBBER:		return "BTN_TOOL_RUBBER";
	case BTN_TOOL_BRUSH:		return "BTN_TOOL_BRUSH";
	case BTN_TOOL_PENCIL:		return "BTN_TOOL_PENCIL";
	case BTN_TOOL_AIRBRUSH:		return "BTN_TOOL_AIRBRUSH";
	case BTN_TOOL_FINGER:		return "BTN_TOOL_FINGER";
	case BTN_TOOL_MOUSE:		return "BTN_TOOL_MOUSE";
	case BTN_TOOL_LENS:		return "BTN_TOOL_LENS";
	case BTN_TOOL_QUINTTAP:		return "BTN_TOOL_QUINTTAP";
	case BTN_TOOL_DOUBLETAP:	return "BTN_TOOL_DOUBLETAP";
	case BTN_TOOL_TRIPLETAP:	return "BTN_TOOL_TRIPLETAP";
	case BTN_TOOL_QUADTAP:		return "BTN_TOOL_QUADTAP";
	case BTN_TOUCH:			return "BTN_TOUCH";
	case BTN_LEFT:			return "BTN_LEFT";
	case BTN_RIGHT:			return "BTN_RIGHT";
	case BTN_MIDDLE:		return "BTN_MIDDLE";
	case BTN_SIDE:			return "BTN_SIDE";
	case BTN_EXTRA:			return "BTN_EXTRA";
	case BTN_FORWARD:		return "BTN_FORDWARD";
	case BTN_BACK:			return "BTN_BACK";
	case BTN_TASK:			return "BTN_TASK";
	default:
	    snprintf(buffer, sizeof(buffer), "%d", code);
	    return buffer;
    }
}

static const char *_ev_code_rel(uint16_t code)
{
    switch (code)
    {
	case REL_X:		return "REL_X";
	case REL_Y:		return "REL_Y";
	case REL_Z:		return "REL_Z";
	case REL_RX:		return "REL_RX";
	case REL_RY:		return "REL_RY";
	case REL_RZ:		return "REL_RZ";
	case REL_HWHEEL:	return "REL_HWHEEL";
	case REL_DIAL:		return "REL_DIAL";
	case REL_WHEEL:		return "REL_WHEEL";
	case REL_MISC:		return "REL_MISC";
	default:		return "???";
    }
}

static const char *_ev_code_abs(uint16_t code)
{
    switch (code)
    {
	case ABS_DISTANCE:		return "ABS_DISTANCE";
	case ABS_MT_SLOT:		return "ABS_MT_SLOT";
	case ABS_MT_TOUCH_MAJOR:	return "ABS_MT_TOUCH_MAJOR";
	case ABS_MT_TOUCH_MINOR:	return "ABS_MT_TOUCH_MINOR";
	case ABS_MT_WIDTH_MAJOR:	return "ABS_MT_WIDTH_MAJOR";
	case ABS_MT_WIDTH_MINOR:	return "ABS_MT_WIDTH_MINOR";
	case ABS_MT_ORIENTATION:	return "ABS_MT_ORIENTATION";
	case ABS_MT_POSITION_X:		return "ABS_MT_POSITION_X";
	case ABS_MT_POSITION_Y:		return "ABS_MT_POSITION_Y";
	case ABS_MT_TOOL_TYPE:		return "ABS_MT_TOOL_TYPE";
	case ABS_MT_BLOB_ID:		return "ABS_MT_BLOB_ID";
	case ABS_MT_TRACKING_ID:	return "ABS_MT_TRACKING_ID";
	case ABS_MT_PRESSURE:		return "ABS_MT_PRESSURE";
	case ABS_MT_DISTANCE:		return "ABS_MT_DISTANCE";
	case ABS_MT_TOOL_X:		return "ABS_MT_TOOL_X";
	case ABS_MT_TOOL_Y:		return "ABS_MT_TOOL_Y";
	default:			return "???";
    }
}

static const char *_ev_code_msc(uint16_t code)
{
    switch (code)
    {
	case MSC_TIMESTAMP:	return "MSC_TIMESTAMP";
	case MSC_PULSELED:	return "MSC_PULSELED";
	case MSC_SERIAL:	return "MSC_SERIAL";
	case MSC_GESTURE:	return "MSC_GESTURE";
	case MSC_RAW:		return "MSC_RAW";
	case MSC_SCAN:		return "MSC_SCAN";
	case MSC_MAX:		return "MSC_MAX";
	case MSC_CNT:		return "MSC_CNT";
	default:		return "???";
    }
}

static const char *_ev_code_sw(uint16_t code)
{
    switch (code)
    {
	case SW_LID:		return "SW_LID";
	default:		return "???";
    }
}

static const char *_ev_code_led(uint16_t code)
{
    switch (code)
    {
	default:	return "???";
    }
}

static const char *_ev_code_snd(uint16_t code)
{
    switch (code)
    {
	default:	return "???";
    }
}

static const char *_ev_code_rep(uint16_t code)
{
    switch (code)
    {
	default:	return "???";
    }
}

static const char *_ev_code_ff(uint16_t code)
{
    switch (code)
    {
	default:	return "???";
    }
}

static const char *_ev_code_pwr(uint16_t code)
{
    switch (code)
    {
	default:	return "???";
    }
}

static const char *_ev_code_ff_status(uint16_t code)
{
    switch (code)
    {
	default:	return "???";
    }
}

const char* twtk_evdev_get_event_code_name(uint16_t type, uint16_t code)
{
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
	default:		return "???";
    }
}

struct evdev_state
{
    pthread_t			thread;
    twtk_vector_t		last_position;
    twtk_event_mouse_button_t	last_buttons;
    int				fd;
    int				id;
    struct libevdev		*dev;
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

static int _handle_mouse_event(struct evdev_state *state, struct input_event *ev)
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

	case EV_KEY:
	{
	    event.mouse.button = evcode_to_button(ev->code);
	    event.mouse.absolute = state->last_position;

	    if (ev->value)
	    {
		state->last_buttons |= event.mouse.button;
		event.mouse.event = TWTK_EVENT_MOUSE_BTN_DOWN;
	    }
	    else
	    {
		state->last_buttons &= ~event.mouse.button;
		event.mouse.event = TWTK_EVENT_MOUSE_BTN_UP;
	    }

	    event.mouse.all_buttons = state->last_buttons;
	    return twtk_event_raise(&event);
	}

	case EV_SYN:
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

void twtk_evdev_mouse_loop(struct evdev_state* state)
{
    int rc = 0;
    do {
	struct input_event ev;
	if ((rc = libevdev_next_event(state->dev, LIBEVDEV_READ_FLAG_NORMAL, &ev)) == 0)
	{
	    if (!_handle_mouse_event(state, &ev))
	    {
		_DEBUG("Event: %-10s %-10s %5d",
		    twtk_evdev_get_event_type_name(ev.type),
		    twtk_evdev_get_event_code_name(ev.type, ev.code),
		    ev.value);
	    }
	}
    } while (rc == 1 || rc == 0 || rc == -EAGAIN);
    free(state);
}

static void *_mouse_thread(void *ptr)
{
    twtk_evdev_mouse_loop((struct evdev_state*)ptr);
}

void twtk_evdev_mouse_start(const char* devname)
{
    struct evdev_state *state = calloc(1, sizeof(struct evdev_state));

    static last_device_id = 0;

    state->id = ++last_device_id;

    if ((state->fd = open(devname, O_RDONLY | O_NONBLOCK)) == -1) {
	_DEBUG("failed opening mouse device: %s", strerror(errno));
	return;
    }

    int rc = libevdev_new_from_fd (state->fd, &state->dev);

    if (state->dev == NULL)
    {
	_DEBUG("failed opening evdev: %s", strerror(-rc));
	return;
    }

    _DEBUG("Input device name: \"%s\"", libevdev_get_name(state->dev));
    _DEBUG("Input device ID: bus %#x vendor %#x product %#x",
        libevdev_get_id_bustype(state->dev),
        libevdev_get_id_vendor(state->dev),
        libevdev_get_id_product(state->dev));

    if (!libevdev_has_event_type(state->dev, EV_REL) ||
        !libevdev_has_event_code(state->dev, EV_KEY, BTN_LEFT)) {
	    _DEBUG("This device does not look like a mouse");
	    return;
    }

    int res = pthread_create(&state->thread, NULL, _mouse_thread, state);
    if (!res)
	_DEBUG("thread creation failed: %s", strerror(errno));
}
