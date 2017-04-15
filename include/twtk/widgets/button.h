#ifndef __TWTK_WIDGET_BUTTON_H
#define __TWTK_WIDGET_BUTTON_H

#include <twtk/str.h>
#include <twtk/types.h>
#include <twtk/rect.h>

twtk_widget_t *twtk_button_widget_create(twtk_str_t filename,
                                         twtk_rect_t rect,
                                         twtk_str_t signal);

#endif /* __TWTK_WIDGET_BUTTON_H */
