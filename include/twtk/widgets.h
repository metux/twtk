#ifndef __TWTK_WIDGETS_H
#define __TWTK_WIDGETS_H

twtk_widget_t *twtk_text_widget_create(const char* text, double x, double y, double w, double h);
twtk_widget_t *twtk_image_widget_create(const char* filename, double x, double y, double w, double h);
twtk_widget_t *twtk_window_widget_create(double x, double y, double w, double h);
twtk_widget_t *twtk_pattern_widget_create(double x, double y, double w, double h);

#endif /* __TWTK_WIDGETS_H */
