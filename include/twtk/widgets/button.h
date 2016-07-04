#ifndef __TWTK_WIDGET_BUTTON_H
#define __TWTK_WIDGET_BUTTON_H

#include <twtk/types.h>
#include <twtk/rect.h>

twtk_widget_t *twtk_button_widget_create(const char* filename,
                                         twtk_rect_t rect,
                                         const char* signal)
    __attribute__((nonnull(1)));

#endif /* __TWTK_WIDGET_BUTTON_H */
