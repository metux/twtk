#ifndef __TWTK_WIDGET_MOVEBOX_H
#define __TWTK_WIDGET_MOVEBOX_H

#include <twtk/types.h>

/**
 * A movebox just holds one child/subframe widget and allows moving
 * the child.
 */
twtk_widget_t *twtk_movebox_widget_create(twtk_widget_t *widget)
    __attribute__((nonnull(1)));

#endif /* __TWTK_WIDGET_MOVEBOX_H */
