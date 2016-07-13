#ifndef __TWTK_WIDGETS_H
#define __TWTK_WIDGETS_H

twtk_widget_t *twtk_text_widget_create(const char* text, twtk_dim_t x, twtk_dim_t y, twtk_dim_t w, twtk_dim_t h);
twtk_widget_t *twtk_image_widget_create(const char* filename, twtk_dim_t x, twtk_dim_t y, twtk_dim_t w, twtk_dim_t h);
twtk_widget_t *twtk_window_widget_create(twtk_dim_t x, twtk_dim_t y, twtk_dim_t w, twtk_dim_t h);
twtk_widget_t *twtk_pattern_widget_create(twtk_dim_t x, twtk_dim_t y, twtk_dim_t w, twtk_dim_t h);

#endif /* __TWTK_WIDGETS_H */
