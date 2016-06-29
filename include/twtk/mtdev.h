#ifndef __TWTK_MTDEV_H
#define __TWTK_MTDEV_H

#include <inttypes.h>
#include <twtk/events.h>

const char* twtk_mtdev_get_event_type_name(uint16_t type);
const char* twtk_mtdev_get_event_code_name(uint16_t type, uint16_t code);

void twtk_mtdev_mouse_start(const char* devname);

#endif /* __TWTK_MTDEV_H */
