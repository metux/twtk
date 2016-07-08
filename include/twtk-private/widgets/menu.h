#ifndef __TWTK_PRIVATE_MENU_H
#define __TWTK_PRIVATE_MENU_H

#include <assert.h>
#include <stdbool.h>
#include <cairo.h>
#include <twtk/fontspec.h>
#include <twtk/types.h>
#include <twtk/rect.h>
#include <twtk/menu.h>
#include <twtk/widget.h>
#include <twtk/color.h>

#define TWTK_MENU_WIDGET_MAGIC		0x454d554e30303030 // "MENU0000"
#define TWTK_MENU_WIDGET_BORDER_WIDTH	1
#define TWTK_MENU_WIDGET_BORDER_COLOR	TWTK_COLOR_RED
#define TWTK_MENU_WIDGET_SPACING	1


typedef struct
{
    twtk_rect_t rect;
} twtk_menu_widget_entry_t;

typedef struct
{
    const char *name;
    twtk_fontspec_t fontspec;
    twtk_color_t border_color;
    twtk_dim_t border_width;
    twtk_dim_t padding_x;
    twtk_dim_t padding_y;
} twtk_menu_widget_mode_style_t;

typedef struct
{
    twtk_menu_widget_mode_style_t plain;
    twtk_menu_widget_mode_style_t hover;
} twtk_menu_widget_style_t;

typedef struct
{
    twtk_menu_t *menu;
    int entry_count;
    twtk_menu_widget_entry_t *entries;
    int hover_entry;
    int open_entry;
    twtk_vector_t text_padding;
    twtk_widget_t *widget;
    twtk_dim_t border_width;
    const twtk_menu_widget_style_t *style;
} twtk_menu_widget_priv_t;


extern const twtk_widget_class_t twtk_menu_widget_class;

static inline twtk_menu_widget_priv_t *twtk_menu_widget_priv(twtk_widget_t *widget)
{
    assert(widget);
    assert(widget->cls);
    assert(widget->cls->magic == TWTK_MENU_WIDGET_MAGIC);
    return (twtk_menu_widget_priv_t*)widget->priv;
}

/** return the currently hover'ed menu entry **/
static inline twtk_menu_entry_t *twtk_menu_widget_current_ment(twtk_widget_t *widget)
{
    assert(widget);
    twtk_menu_widget_priv_t *priv = twtk_menu_widget_priv(widget);
    assert(priv->menu);
    assert(priv->menu->entries);
    return &priv->menu->entries[priv->hover_entry];
}

static inline twtk_menu_widget_entry_t *twtk_menu_widget_current_went(twtk_widget_t *widget)
{
    assert(widget);
    twtk_menu_widget_priv_t *priv = twtk_menu_widget_priv(widget);
    assert(priv->entries);
    return &priv->entries[priv->hover_entry];
}

twtk_widget_t *twtk_menu_widget_alloc(twtk_menu_t *menu)
    __attribute__((nonnull(1)));

twtk_widget_t *twtk_menu_widget_alloc_sub(twtk_widget_t* widget, twtk_menu_t *menu)
    __attribute__((nonnull(1,2)));

int twtk_menu_widget_close_submenu(twtk_widget_t *widget)
    __attribute__((nonnull(1)));

int twtk_menu_widget_open_submenu(twtk_widget_t *widget)
    __attribute__((nonnull(1)));

int twtk_menu_widget_action(twtk_widget_t *widget)
    __attribute__((nonnull(1)));

int twtk_menu_widget_op_paint(twtk_widget_t *widget, cairo_t *cr)
    __attribute__((nonnull(1,2)));

int twtk_menu_widget_update_boxes(twtk_widget_t *widget)
    __attribute__((nonnull(1)));

int twtk_menu_widget_op_event_mouse(twtk_widget_t *widget, twtk_event_t *event)
    __attribute__((nonnull(1,2)));

#endif /* __TWTK_PRIVATE_MENU_H */
