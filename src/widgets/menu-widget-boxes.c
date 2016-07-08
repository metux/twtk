
#define _DEBUG_NAME	"menu-widget"

#ifdef ENABLE_DEBUG_WIDGET_MENU
#define ENABLE_DEBUG
#endif

#include <errno.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>
#include <twtk/fontspec.h>
#include <twtk/widget.h>
#include <twtk/types.h>
#include <twtk/menu.h>
#include <twtk-private/cairo_util.h>
#include <twtk-private/debug.h>
#include <twtk-private/widgets/menu.h>


static void _scandim(
    const twtk_menu_widget_mode_style_t *mode,
    twtk_menu_widget_priv_t *priv,
    twtk_dim_t *r_bbox_w,
    twtk_dim_t *r_bbox_h,
    twtk_vector_t *r_padding
)
{
    for (int x=0; x<priv->entry_count; x++)
    {
        cairo_text_extents_t ext;
        twtk_fontspec_text_extents(&mode->fontspec, priv->menu->entries[x].text, &ext);

        r_bbox_w[x]    = twtk_dim_max(r_bbox_w[x],    ext.width  + ext.x_bearing + mode->padding_x*2 + mode->border_width*2);
        r_bbox_h[0]    = twtk_dim_max(r_bbox_h[0],    ext.height + mode->padding_y*2 + mode->border_width*2);
        r_padding->x   = twtk_dim_max(r_padding->x,   mode->border_width + mode->padding_x);
        r_padding->y   = twtk_dim_max(r_padding->y,   -ext.y_bearing + mode->border_width + mode->padding_y);

        _DEBUG("%s[%d]: width: %4.0f  height: %4.0f  xadv: %4.0f  yadv: %4.0f  xb: %4.0f  yb: %4.0f  YPOS: %4.0f",
            mode->name,
            x,
            ext.width,
            ext.height,
            ext.x_advance,
            ext.y_advance,
            ext.x_bearing,
            ext.y_bearing,
            r_padding->y
        );
    }
}

int twtk_menu_widget_update_boxes(twtk_widget_t *widget)
{
    assert(widget);

    twtk_menu_widget_priv_t *priv = (twtk_menu_widget_priv_t*)widget->priv;

    if (priv->entries != NULL)
        return -ENOENT;

    _DEBUG("creating boxes");

    // fixme: move that do its own functions, to reflect menu updates
    // fixme: we probably need a callback for that
    int cnt = priv->entry_count = priv->menu->count;
    priv->entries = TWTK_ALLOC_ARRAY(twtk_menu_widget_entry_t,cnt);

    twtk_dim_t bbox_h = 0;
    twtk_dim_t bbox_w[cnt];
    twtk_dim_t pad_y = 0;

    for (int x=0; x<cnt; x++)
        bbox_w[cnt] = 0;

    twtk_vector_t vsize = TWTK_VECTOR(priv->border_width, 0);

    _scandim(&priv->style->plain, priv, (twtk_dim_t *)&bbox_w, &vsize.y, &priv->text_padding);
    _scandim(&priv->style->hover, priv, (twtk_dim_t *)&bbox_w, &vsize.y, &priv->text_padding);

    for (int x=0; x<cnt; x++)
    {
        priv->entries[x].rect = twtk_rect_by_coords(vsize.x, priv->border_width, bbox_w[x], vsize.y, 0);

        _DEBUG("  rect #%d x=%4.0f y=%4.0f w=%4.0f h=%4.0f",
            x,
            priv->entries[x].rect.pos.x,
            priv->entries[x].rect.pos.y,
            priv->entries[x].rect.size.x,
            priv->entries[x].rect.size.y
        );

        vsize.x += priv->entries[x].rect.size.x + TWTK_MENU_WIDGET_SPACING;
    }

    vsize.x += priv->border_width - TWTK_MENU_WIDGET_SPACING;
    vsize.y += priv->border_width*2;
    twtk_widget_vresize(widget, vsize);

    return 0;
}
