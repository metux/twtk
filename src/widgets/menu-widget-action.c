
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

    twtk_event_t ev = {
        .type = TWTK_EVENT_ACTION,
        .action = {
            .action = TWTK_EVENT_ACTION_PUSH,
            .widget = twtk_widget_ref(widget),
            .signal = twtk_menu_widget_current_ment(widget)->id
        }
    };

    twtk_widget_event(widget->parent, &ev, TWTK_EVENT_DISPATCH_DIRECT);
    twtk_widget_unref(ev.action.widget);

    return 0;
}
