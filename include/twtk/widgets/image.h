#ifndef __TWTK_WIDGET_IMAGE_H
#define __TWTK_WIDGET_IMAGE_H

#include <twtk/types.h>


twtk_widget_t *twtk_image_widget_create(
    const char* filename,
    twtk_dim_t x,
    twtk_dim_t y,
    twtk_dim_t w,
    twtk_dim_t h
);

#endif /* __TWTK_WIDGET_IMAGE_H */
