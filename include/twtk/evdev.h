#ifndef __TWTK_EVDEV_H
#define __TWTK_EVDEV_H

#include <inttypes.h>
#include <twtk/events.h>

const char* twtk_evdev_get_event_type_name(uint16_t type);
const char* twtk_evdev_get_event_code_name(uint16_t type, uint16_t code);

void twtk_evdev_mouse_start(const char* devname);

#endif /* __TWTK_EVDEV_H */
