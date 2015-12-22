
#include <twtk-private/debug-widget.h>

twtk_widget_t *_twtk_debug_widget = NULL;

void twtk_debug_widget_set_text(const char *text)
{
    if (_twtk_debug_widget)
        twtk_widget_set_text(_twtk_debug_widget, text);
}

void twtk_debug_widget_install(twtk_widget_t *widget)
{
    if (_twtk_debug_widget)
        twtk_widget_unref(_twtk_debug_widget);

    if (widget == NULL)
        _twtk_debug_widget = NULL;
    else
        _twtk_debug_widget = twtk_widget_ref(widget);
}

void twtk_debug_widget_redraw()
{
    if (_twtk_debug_widget)
        twtk_platform_redraw_widget(_twtk_debug_widget, "<debug>");
}
