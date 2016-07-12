#ifndef __TWTK_PRIVATE_PLATFORM_GENERIC_H
#define __TWTK_PRIVATE_PLATFORM_GENERIC_H

#include <cairo.h>
#include <twtk/types.h>

cairo_t       *_twtk_platform_generic_get_context(twtk_platform_t *pl);
int            _twtk_platform_generic_free_context(twtk_platform_t *pl, cairo_t *ctx);
twtk_widget_t *_twtk_platform_generic_get_root(twtk_platform_t *pl);

/** map a widget into parent frame **/
int _twtk_platform_generic_map_widget(
    twtk_platform_t *platform,
    twtk_widget_t *widget,
    twtk_widget_t *parent);

#endif /* __TWTK_PRIVATE_PLATFORM_GENERIC_H */
