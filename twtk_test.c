
#include <assert.h>
#include <stdio.h>
#include <cairo.h>
#include <math.h>
#include <twtk/widget.h>
#include <twtk/widgets.h>
#include <twtk/platform.h>
#include <twtk/color.h>
#include <twtk/widgets/image.h>
#include <twtk/widgets/text.h>
#include <twtk/widgets/position.h>
#include <twtk/widgets/movebox.h>
#include <twtk-private/debug-widget.h>


static void _create_position(twtk_widget_t* parent)
{
    return twtk_widget_add_child_unref(
        parent,
        twtk_movebox_widget_create(twtk_position_widget_create(100, 100)),
        "position"
    );
}

static twtk_widget_t *_create_red_box()
{
    twtk_widget_t *widget = twtk_text_widget_create(NULL, 0, 0, 100, 100);
    twtk_widget_set_border_color(widget, TWTK_COLOR_RED);
    twtk_widget_set_border_width(widget, 1);
    return widget;
}

static twtk_widget_t *_create_blue_box()
{
    twtk_widget_t *widget = twtk_text_widget_create(NULL, 0, 100, 100, 100);
    twtk_widget_set_border_color(widget, TWTK_COLOR_BLUE);
    twtk_widget_set_border_width(widget, 1);
    return widget;
}

static twtk_widget_t *_create_green_box()
{
    twtk_widget_t *widget = twtk_text_widget_create(NULL, 0, 200, 100, 100);
    twtk_widget_set_border_color(widget, TWTK_COLOR_GREEN);
    twtk_widget_set_border_width(widget, 1);
    return widget;
}

static twtk_widget_t *_create_text_1()
{
    twtk_widget_t *widget = twtk_text_widget_create("Salamipizza\nWurstbrot", 500, 50, 300, 100);
    twtk_widget_set_font_name(widget, "Sans");
    twtk_widget_set_font_color(widget, TWTK_COLOR_WHITE);
    twtk_widget_set_font_size(widget, 40);
    twtk_widget_set_border_color(widget, TWTK_COLOR_BLACK);
    twtk_widget_set_border_width(widget, 1);
    twtk_widget_set_background_color(widget, TWTK_COLOR_YELLOW);
    return widget;
}

static twtk_widget_t *_create_debug()
{
    twtk_widget_t *widget = twtk_text_widget_create("+++", 500, 150, 300, 100);
    twtk_widget_set_font_name(widget, "Sans");
    twtk_widget_set_font_color(widget, TWTK_COLOR_BLACK);
    twtk_widget_set_border_color(widget, TWTK_COLOR_WHITE);
    twtk_widget_set_border_width(widget, 1);
    twtk_widget_set_background_color(widget, TWTK_COLOR_YELLOW);
    twtk_debug_widget_install(widget);
    return widget;
}

static twtk_widget_t *_create_image_lara()
{
    twtk_widget_t *widget = twtk_image_widget_create("resources/001.jpg", 400, 100, -1, -1);
    return widget;
}

static twtk_widget_t *_create_image_black()
{
    twtk_widget_t *widget = twtk_image_widget_create("resources/002.jpg", 50, 50, -1, -1);
    return widget;
}

static twtk_widget_t *_create_box2()
{
    twtk_widget_t *box = twtk_text_widget_create(NULL, 300, 500, 500, 500);
    twtk_widget_set_border_color(box, TWTK_COLOR_BLUE);
    twtk_widget_set_border_width(box, 5);
    twtk_widget_set_font_size(box, 24);
    return box;
}

static twtk_widget_t *_create_win()
{
    twtk_widget_t *img = twtk_image_widget_create("resources/002.jpg", 30, 30, 0, 0);

    twtk_widget_t *win = twtk_window_widget_create(1000, 400, 200, 200);
    twtk_widget_set_background_color(win, TWTK_COLOR_BLACK);
    twtk_widget_set_border_color(win, TWTK_COLOR_BLACK);
    twtk_widget_set_border_width(win, 30);

    twtk_widget_add_child(win, img, "image1");

    return win;
}

static void _init_boxes()
{
    twtk_widget_t *root = twtk_platform_get_root();

    twtk_widget_set_background_color(root, TWTK_COLOR_GREY);

    twtk_widget_add_child_unref(root, _create_red_box(),     "red");
    twtk_widget_add_child_unref(root, _create_blue_box(),    "blue");
    twtk_widget_add_child_unref(root, _create_green_box(),   "green");
    twtk_widget_add_child_unref(root, _create_text_1(),      "text-1");
    twtk_widget_add_child_unref(root, _create_image_lara(),  "image-lara");
    twtk_widget_add_child_unref(root, _create_image_black(), "image-black");
    twtk_widget_add_child_unref(root, _create_box2(),        "box2");
    twtk_widget_add_child_unref(root, _create_debug(),       "debug");
    twtk_widget_add_child_unref(root, _create_win(),         "win");

    _create_position(root);
}

int main(void)
{
    twtk_platform_setup();

    _init_boxes();

    twtk_platform_redraw();
    twtk_platform_loop();
    twtk_platform_finish();

    return 0;
}
