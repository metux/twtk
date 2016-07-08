
#define _DEBUG_NAME	"menu-widget"

#ifdef ENABLE_DEBUG_WIDGET_MENU
#define ENABLE_DEBUG
#endif

//#include <errno.h>
#include <assert.h>
#include <twtk/events.h>
#include <twtk/widget.h>
#include <twtk/types.h>
#include <twtk/menu.h>
#include <twtk-private/debug.h>
#include <twtk-private/widgets/menu.h>

#include <twtk/widget-helper.h>


#define MAGIC			TWTK_MENU_WIDGET_MAGIC

#define _WIDGET_PRIV_TYPE	twtk_menu_widget_priv_t
#define _WIDGET_CLASS_INF	twtk_menu_widget_class

static const twtk_menu_widget_style_t style = {
    .plain = {
        .name         = "plain",
        .fontspec     = {
            .name   = "monospace",
            .size   = 20,
            .slant  = CAIRO_FONT_SLANT_NORMAL,
            .weight = CAIRO_FONT_WEIGHT_NORMAL,
            .color  = TWTK_COLORVAL_BLACK
        },
        .border_color = TWTK_COLORVAL_BLUE,
        .border_width = 1,
        .padding_x    = 2,
        .padding_y    = 2,
    },
    .hover = {
        .name         = "hover",
        .fontspec = {
            .name   = "monospace",
            .size   = 15,
            .slant  = CAIRO_FONT_SLANT_ITALIC,
            .weight = CAIRO_FONT_WEIGHT_BOLD,
            .color  = TWTK_COLORVAL_GREEN
        },
        .border_color = TWTK_COLORVAL_YELLOW,
        .border_width = 2,
        .padding_x    = 2,
        .padding_y    = 2
    }
};


static int _op_fini(twtk_widget_t *widget);
static int _op_event(twtk_widget_t *widget, twtk_event_t *event, twtk_event_dispatch_t d);

const twtk_widget_class_t twtk_menu_widget_class = {
    .magic        = TWTK_MENU_WIDGET_MAGIC,
    .op_fini      = _op_fini,
    .op_event     = _op_event,
    .op_paint     = twtk_menu_widget_op_paint,
    .priv_size    = sizeof(twtk_menu_widget_priv_t)
};

twtk_widget_t *twtk_menu_widget_alloc(twtk_menu_t *menu)
{
    twtk_widget_t *widget = twtk_widget_alloc(&twtk_menu_widget_class);
    twtk_menu_widget_priv_t *priv= (twtk_menu_widget_priv_t*)(widget->priv);
    priv->menu = twtk_menu_ref(menu);
    priv->hover_entry = -1;
    priv->border_width = TWTK_MENU_WIDGET_BORDER_WIDTH;
    priv->style = &style;
    twtk_menu_widget_update_boxes(widget);

    return widget;
}

twtk_widget_t *twtk_menu_widget_alloc_sub(twtk_widget_t *parent, twtk_menu_t *menu)
{
    assert(parent);
    assert(menu);

    twtk_widget_t *sub = twtk_menu_widget_alloc(menu);
    twtk_menu_widget_priv_t *p_priv = twtk_menu_widget_priv(parent);
    twtk_menu_widget_priv_t *s_priv = twtk_menu_widget_priv(sub);

    s_priv->border_width = p_priv->border_width;

    return sub;
}

static int _op_fini(twtk_widget_t *widget)
{
    TWTK_WIDGET_OP_BEGIN

    if (priv->entries)
        free(priv->entries);

    TWTK_WIDGET_OP_END(0);
}

static int _op_event(twtk_widget_t *widget, twtk_event_t *event, twtk_event_dispatch_t d)
{
    assert(widget);
    assert(event);

    TWTK_WIDGET_OP_BEGIN

    if ((d == TWTK_EVENT_DISPATCH_DIRECT) && (event->type == TWTK_EVENT_MOUSE))
        TWTK_WIDGET_OP_RETURN(twtk_menu_widget_op_event_mouse(widget, event));

    _DEBUG("non-mouse event: %s", twtk_event_type_name(event->type));

    TWTK_WIDGET_OP_END(0);
}

twtk_widget_t *twtk_menu_widget_create(twtk_menu_t *menu, twtk_rect_t rect)
{
    assert(menu);

    twtk_widget_t *widget = twtk_menu_widget_alloc(menu);
    rect = twtk_rect_size_if_null_v(rect, widget->surface.size);

    twtk_widget_resize(widget, rect.size);
    twtk_widget_move(widget, rect.pos);

    return widget;
}
