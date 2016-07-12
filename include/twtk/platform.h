#ifndef __TWTK_PLATFORM_H
#define __TWTK_PLATFORM_H

#include <stdlib.h>
#include <cairo.h>
#include <twtk/types.h>

extern twtk_platform_t *_twtk_current_platform;

struct __twtk_platform {
    cairo_surface_t *surface;
    cairo_device_t *device;
    twtk_widget_t *root;

    int            (*op_destroy)     (twtk_platform_t *platform);
    twtk_widget_t *(*op_get_root)    (twtk_platform_t *platform);
    cairo_t       *(*op_get_context) (twtk_platform_t *platform);
    int            (*op_free_context)(twtk_platform_t *platform, cairo_t *cr);
    int            (*op_mainloop)    (twtk_platform_t *platform);
    int            (*op_stop)        (twtk_platform_t *platform);
    int            (*op_map_widget)  (twtk_platform_t *platform, twtk_widget_t *widget, twtk_widget_t *parent);
};

static inline twtk_widget_t *twtk_platform_get_root()
{
    if (_twtk_current_platform == NULL)
	return NULL;

    return _twtk_current_platform->op_get_root(_twtk_current_platform);
}

void twtk_platform_install(twtk_platform_t *platform);
void twtk_platform_setup();
void twtk_platform_finish();
void twtk_platform_redraw();
void twtk_platform_redraw_widget(twtk_widget_t *widget, const char *name);
void twtk_platform_loop();
void twtk_platform_init_dispatch();
void twtk_platform_stop();

int  twtk_platform_map_widget(twtk_widget_t *widget, twtk_widget_t *parent)
    __attribute__((nonnull(1)));

#endif /* __TWTK_PLATFORM_H */
