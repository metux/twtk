#ifndef __TWTK_WIDGET_MENU_H
#define __TWTK_WIDGET_MENU_H

#include <twtk/types.h>
#include <twtk/rect.h>


twtk_widget_t *twtk_menu_widget_create(twtk_menu_t *menu, twtk_rect_t rect)
    __attribute__((nonnull(1)));

#endif /* __TWTK_WIDGET_MENU_H */
