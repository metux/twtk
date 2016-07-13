#ifndef __TWTK_WIDGET_TEXT_H
#define __TWTK_WIDGET_TEXT_H

#include <twtk/types.h>


twtk_widget_t *twtk_text_widget_create(
    const char* text,
    twtk_dim_t x,
    twtk_dim_t y,
    twtk_dim_t w,
    twtk_dim_t h
);

#endif /* __TWTK_WIDGET_TEXT_H */
