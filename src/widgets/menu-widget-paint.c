
#define _DEBUG_NAME	"menu-widget"

#ifdef ENABLE_DEBUG_WIDGET_MENU
#define ENABLE_DEBUG
#endif

#include <cairo.h>
#include <assert.h>
#include <twtk/fontspec.h>
#include <twtk/widget.h>
#include <twtk/types.h>
#include <twtk/menu.h>
#include <twtk/platform.h>
#include <twtk-private/cairo_util.h>
#include <twtk-private/debug.h>
#include <twtk-private/widgets/menu.h>

#include <twtk/widget-helper.h>


#define MAGIC			TWTK_MENU_WIDGET_MAGIC

#define _WIDGET_PRIV_TYPE	twtk_menu_widget_priv_t
#define _WIDGET_CLASS_INF	twtk_menu_widget_class


int twtk_menu_widget_op_paint(twtk_widget_t *widget, cairo_t *cr)
{
    TWTK_WIDGET_OP_BEGIN

    assert (cr);

    {
        _twtk_ut_set_rgba (cr, TWTK_MENU_WIDGET_BORDER_COLOR);
        cairo_set_line_width (cr, priv->border_width);
        _twtk_ut_rect_to_vec (cr, widget->viewport.size);
        cairo_stroke (cr);
    }

    for (int x=0; x<priv->entry_count; x++)
    {
        twtk_rect_t rect = priv->entries[x].rect;
        const twtk_menu_widget_mode_style_t *mode = ((priv->hover_entry == x) ?
            &priv->style->hover :
            &priv->style->plain);

        cairo_move_to(cr, rect.pos.x+priv->text_padding.x, priv->text_padding.y);
        twtk_fontspec_select_cr(&mode->fontspec, cr);
        cairo_show_text(cr, priv->menu->entries[x].text);

        _twtk_ut_set_rgba(cr, mode->border_color);
        cairo_set_line_width(cr, mode->border_width);

        // paint inner rect
        cairo_rectangle(
            cr,
            rect.pos.x+mode->border_width/2,
            rect.pos.y+mode->border_width/2,
            rect.size.x-mode->border_width,
            rect.size.y-mode->border_width
        );

        cairo_stroke (cr);
    }

    TWTK_WIDGET_OP_END(0);
}
