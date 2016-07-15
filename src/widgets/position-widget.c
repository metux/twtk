
#include <math.h>
#include <assert.h>
#include <twtk/events.h>
#include <twtk/widget.h>
#include <twtk/platform.h>
#include <twtk/color.h>
#include <twtk/widgets/text.h>


static inline long _do_round(twtk_dim_t x)
{
    return (long) (x+0.5);
}

static int _handler(twtk_event_t *ev, void *priv)
{
    char buffer[4096] = { 0 };
    snprintf(buffer,
        sizeof(buffer)-1,
        "%4ld : %4ld %s %s %s",
        _do_round(ev->mouse.pos.x),
        _do_round(ev->mouse.pos.y),
        (ev->mouse.all_buttons & TWTK_EVENT_MOUSE_BUTTON_LEFT)   ? "L" : " ",
        (ev->mouse.all_buttons & TWTK_EVENT_MOUSE_BUTTON_MIDDLE) ? "M" : " ",
        (ev->mouse.all_buttons & TWTK_EVENT_MOUSE_BUTTON_RIGHT)  ? "R" : " ");

    twtk_widget_t *pos_widget = (twtk_widget_t*)priv;

    twtk_widget_set_str(pos_widget, "text", buffer);
    twtk_widget_dirty(pos_widget);
}

#define WIDGET_SIZE_X	250
#define WIDGET_SIZE_Y	30

twtk_widget_t *twtk_position_widget_create(twtk_dim_t x, twtk_dim_t y)
{
    /* create the position widget */
    twtk_widget_t *pos_widget = twtk_text_widget_create(NULL, x, y, WIDGET_SIZE_X, WIDGET_SIZE_Y);

    twtk_widget_set_border_color(pos_widget, TWTK_COLOR_RED);
    twtk_widget_set_background_color(pos_widget, TWTK_COLOR_GREEN);
    twtk_widget_set_font_color(pos_widget, TWTK_COLOR_BLUE);
    twtk_widget_set_border_width(pos_widget, 5);

    twtk_event_notify("position-widget", _handler, pos_widget);

    return pos_widget;
}
