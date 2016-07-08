
#define _DEBUG_NAME	"menu-widget"

#ifdef ENABLE_DEBUG_WIDGET_MENU
#define ENABLE_DEBUG
#endif

#include <errno.h>
#include <stdlib.h>
#include <assert.h>
#include <twtk/events.h>
#include <twtk/widget.h>
#include <twtk/types.h>
#include <twtk/menu.h>
#include <twtk-private/debug.h>
#include <twtk-private/widgets/menu.h>


int twtk_menu_widget_action(twtk_widget_t *widget)
{
    assert(widget);

    if (widget->parent == NULL)
        return -ENOENT;

    twtk_menu_widget_priv_t *priv = twtk_menu_widget_priv(widget);

    assert(priv->hover_entry != -1);
    assert(priv->entries);

    _DEBUG("sending button push event");

    twtk_widget_notify_action(widget, TWTK_EVENT_ACTION_PUSH, twtk_menu_widget_current_ment(widget)->id);

    return 0;
}
