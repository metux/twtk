
#define _DEBUG_NAME	"menu-widget"

#ifdef ENABLE_DEBUG_WIDGET_MENU
#define ENABLE_DEBUG
#endif

#define _GNU_SOURCE

#include <errno.h>
#include <stdlib.h>
#include <assert.h>
#include <twtk/types.h>
#include <twtk/menu.h>
#include <twtk/platform.h>
#include <twtk-private/debug.h>
#include <twtk-private/widgets/menu.h>


int twtk_menu_widget_close_submenu(twtk_widget_t *widget)
{
    twtk_menu_widget_priv_t *priv = twtk_menu_widget_priv(widget);

    if (priv->widget == NULL)
    {
        _DEBUG("no submenu to close");
        return -ENOENT;
    }

    _DEBUG("closing open menu");

    twtk_platform_map_widget(priv->widget, NULL);
    twtk_widget_set_parent(priv->widget, NULL);
    twtk_widget_unref(priv->widget);
    priv->widget = NULL;

    return 0;
}

int twtk_menu_widget_open_submenu(twtk_widget_t *widget)
{
    twtk_menu_widget_priv_t *priv = twtk_menu_widget_priv(widget);

    assert(priv);
    assert(priv->hover_entry != -1);

    int id = priv->hover_entry;
    twtk_menu_widget_entry_t *stat_ent = &priv->entries[id];
    twtk_menu_entry_t *menu_ent = &priv->menu->entries[id];

    if (menu_ent->sub == NULL)
        return -ENOENT;

    if (priv->widget)
    {
        _DEBUG("submenu already created");
        return 0;
    }

    twtk_widget_t *submenu = twtk_menu_widget_alloc(menu_ent->sub);
    priv->widget = submenu;
    submenu->flags |= TWTK_WIDGET_FLAG_POPUP;
    twtk_menu_widget_priv_t *subpriv = twtk_menu_widget_priv(submenu);
    subpriv->border_width = priv->border_width;

    _DEBUG("entry dims: %.0f:%.0f %.0f:%.0f",
        stat_ent->rect.pos.x,
        stat_ent->rect.pos.y,
        stat_ent->rect.size.x,
        stat_ent->rect.size.y);

    char buffer[128];
    snprintf(buffer, sizeof(buffer), "submenu-%d", id);
    twtk_widget_set_name(submenu, buffer);

    twtk_widget_set_viewport(
        submenu,
        twtk_rect_by_vectors(
            TWTK_VECTOR(
                stat_ent->rect.pos.x - priv->border_width,
                stat_ent->rect.pos.y + stat_ent->rect.size.y
            ),
            submenu->surface.size,
            0
        )
    );

    twtk_widget_add_child(widget, submenu, NULL);

    twtk_widget_unref(submenu);

    return 0;
}
