#ifndef __TWTK_PRIVATE_PLATFORM_GENERIC_H
#define __TWTK_PRIVATE_PLATFORM_GENERIC_H

#include <cairo.h>
#include <twtk/types.h>

cairo_t       *_twtk_platform_generic_get_context(twtk_platform_t *pl);
int            _twtk_platform_generic_free_context(twtk_platform_t *pl, cairo_t *ctx);
twtk_widget_t *_twtk_platform_generic_get_root(twtk_platform_t *pl);

#endif /* __TWTK_PRIVATE_PLATFORM_GENERIC_H */
