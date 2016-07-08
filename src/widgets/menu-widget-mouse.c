
#define _DEBUG_NAME	"menu-widget"

#ifdef ENABLE_DEBUG_WIDGET_MENU
#define ENABLE_DEBUG
#endif

#include <assert.h>
#include <twtk/events.h>
#include <twtk/widget.h>
#include <twtk/types.h>
#include <twtk/menu.h>
#include <twtk-private/debug.h>
#include <twtk-private/widgets/menu.h>


static bool _update_hover(twtk_widget_t *widget, twtk_menu_widget_priv_t *priv, twtk_event_t *event)
{
    /** no entries at all **/
    if (priv->entries == NULL)
    {
        _DEBUG("hover: no entries");
        priv->hover_entry = -1;
        return true;
    }

    /** still inside the same entry **/
    if ((priv->hover_entry >= 0) &&
        (priv->hover_entry < priv->entry_count) &&
        (twtk_vector_in_rect(event->mouse.pos, priv->entries[priv->hover_entry].rect)))
    {
        return false;
    }

    /** look for another entry we might be in now **/
    for (int x=0; x<priv->entry_count; x++)
    {
        if (twtk_vector_in_rect(event->mouse.pos, priv->entries[x].rect))
        {
            _DEBUG("hover: changed from %d to %d", priv->hover_entry, x);
            priv->hover_entry = x;
            return true;
        }
    }

    /** was inside an entry, now outside **/
    if (priv->hover_entry != -1)
    {
        _DEBUG("hover: left");
        priv->hover_entry = -1;
        return true;
    }

    /** still outside an entry **/
    return false;
}

static int _op_event_mouse_move(twtk_widget_t *widget, twtk_menu_widget_priv_t *priv, twtk_event_t *event)
{
    if (_update_hover(widget, priv, event))
        return TWTK_EVSTAT_CONSUMED | TWTK_EVSTAT_DIRTY;
    else
        return TWTK_EVSTAT_CONSUMED;
}

static int _op_event_mouse_up(twtk_widget_t *widget, twtk_menu_widget_priv_t *priv, twtk_event_t *event)
{
    int old_hover = priv->hover_entry;

    bool hover_updated = _update_hover(widget, priv, event);

    if (event->mouse.button != TWTK_EVENT_MOUSE_BUTTON_LEFT)
        return TWTK_EVSTAT_CONSUMED;

    if (priv->hover_entry == -1)
    {
        _DEBUG("click up @ %f:%f outside", event->mouse.pos.x, event->mouse.pos.y);
        goto out;
    }

    if (priv->menu->entries[priv->hover_entry].sub)
    {
        _DEBUG("clicked into submenu");
        twtk_menu_widget_close_submenu(widget);
        goto out;
    }

    _DEBUG("click up @ %f:%f on %d", event->mouse.pos.x, event->mouse.pos.y, priv->hover_entry);

    twtk_menu_widget_action(widget);

out:
    return (hover_updated ?
        TWTK_EVSTAT_CONSUMED | TWTK_EVSTAT_DIRTY :
        TWTK_EVSTAT_CONSUMED);
}

static int _op_event_mouse_down(twtk_widget_t *widget, twtk_menu_widget_priv_t *priv, twtk_event_t *event)
{
    bool hover_updated = _update_hover(widget, priv, event);

    if (event->mouse.button != TWTK_EVENT_MOUSE_BUTTON_LEFT)
        return TWTK_EVSTAT_CONSUMED;

    if (hover_updated && priv->widget)
    {
        twtk_menu_widget_close_submenu(widget);
        goto out;
    }

    if (priv->hover_entry == -1)
    {
        _DEBUG("click dn @ %f:%f", event->mouse.pos.x, event->mouse.pos.y);
        goto out;
    }

    if (twtk_menu_widget_open_submenu(widget))
        goto out;

out:
    return (hover_updated ?
        TWTK_EVSTAT_CONSUMED | TWTK_EVSTAT_DIRTY :
        TWTK_EVSTAT_CONSUMED);
}

int twtk_menu_widget_op_event_mouse(twtk_widget_t *widget, twtk_event_t *event)
{
    assert(widget);
    assert(event);
    twtk_menu_widget_priv_t *priv = (twtk_menu_widget_priv_t*)widget->priv;
    assert(priv);

    switch (event->mouse.event)
    {
        case TWTK_EVENT_MOUSE_LEAVE:
            _DEBUG("mouse leave");
            priv->hover_entry = -1;
            return TWTK_EVSTAT_CONSUMED | TWTK_EVSTAT_DIRTY;
        break;

        case TWTK_EVENT_MOUSE_BTN_DOWN:
            return _op_event_mouse_down(widget, priv, event);

        case TWTK_EVENT_MOUSE_BTN_UP:
            return _op_event_mouse_up(widget, priv, event);

        case TWTK_EVENT_MOUSE_MOVE:
            return _op_event_mouse_move(widget, priv, event);

        default:
            _DEBUG("unhandled mouse event %s @ %s", twtk_event_mouse_type_name(event->mouse.event), widget->name);
        break;
    }

    return TWTK_EVSTAT_CONSUMED;
}
