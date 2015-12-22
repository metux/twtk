#ifndef __TWTK_PRIVATE_DEBUG_WIDGET_H
#define __TWTK_PRIVATE_DEBUG_WIDGET_H

#include <twtk/types.h>
#include <twtk/widget.h>

extern twtk_widget_t *_twtk_debug_widget;

void twtk_debug_widget_set_text(const char *text);
void twtk_debug_widget_install(twtk_widget_t *widget);
void twtk_debug_widget_redraw();

#endif /* __TWTK_PRIVATE_DEBUG_WIDGET_H */
