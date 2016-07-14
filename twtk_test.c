
#include <assert.h>
#include <stdio.h>
#include <cairo.h>
#include <math.h>
#include <twtk/widget.h>
#include <twtk/platform.h>
#include <twtk/color.h>
#include <twtk/menu.h>
#include <twtk/widgets/image.h>
#include <twtk/widgets/text.h>
#include <twtk/widgets/position.h>
#include <twtk/widgets/movebox.h>
#include <twtk/widgets/window.h>
#include <twtk/widgets/menu.h>
#include <twtk-private/debug-widget.h>
#include <twtk-private/strutil.h>


twtk_menu_entry_t submenu_file_entries[] = {
    TWTK_DECLARE_MENU_STATIC_ENTRY("new", "New"),
    TWTK_DECLARE_MENU_STATIC_ENTRY("open", "Open"),
    TWTK_DECLARE_MENU_STATIC_ENTRY("close", "Close"),
    TWTK_DECLARE_MENU_STATIC_ENTRY("print", "Print"),
};

TWTK_DECLARE_MENU_STATIC(submenu_edit, "file", submenu_file_entries);

twtk_menu_entry_t submenu_edit_entries[] = {
    TWTK_DECLARE_MENU_STATIC_ENTRY("copy", "Copy"),
    TWTK_DECLARE_MENU_STATIC_ENTRY("insert", "Insert"),
    TWTK_DECLARE_MENU_STATIC_ENTRY("delete", "Delete"),
};

TWTK_DECLARE_MENU_STATIC(submenu_file, "file", submenu_edit_entries)

twtk_menu_entry_t menu_main_entries[] = {
    TWTK_DECLARE_MENU_STATIC_SUBMENU("file", "File", &submenu_file),
    TWTK_DECLARE_MENU_STATIC_SUBMENU("edit", "Edit", &submenu_edit),
    TWTK_DECLARE_MENU_STATIC_ENTRY("settings", "settings"),
    TWTK_DECLARE_MENU_STATIC_ENTRY("help", "Help")
};

TWTK_DECLARE_MENU_STATIC(menu_main, "main", menu_main_entries)


static void _create_position(twtk_widget_t* parent)
{
    return twtk_widget_add_child_unref(
        parent,
        twtk_movebox_widget_create(twtk_position_widget_create(100, 100)),
        "position"
    );
}

static void _create_menu(twtk_widget_t *parent, twtk_menu_t *menu)
{
    assert(parent);
    assert(menu);

    twtk_widget_t *mw = twtk_menu_widget_create(
        menu,
        twtk_rect_by_coords(100, 400, -1, -1, 0)
    );

    return twtk_widget_add_child_unref(parent, mw, "mainmenu");
}

static void _create_red_box(twtk_widget_t *parent)
{
    twtk_widget_t *widget = twtk_text_widget_create(NULL, 0, 0, 100, 100);
    twtk_widget_set_border_color(widget, TWTK_COLOR_RED);
    twtk_widget_set_border_width(widget, 1);

    twtk_widget_add_child_unref(
        parent,
        twtk_window_widget_create(widget),
        "red"
    );
}

static void _create_blue_box(twtk_widget_t *parent)
{
    twtk_widget_t *widget = twtk_text_widget_create(NULL, 0, 100, 100, 100);
    twtk_widget_set_border_color(widget, TWTK_COLOR_BLUE);
    twtk_widget_set_border_width(widget, 1);

    twtk_widget_add_child_unref(
        parent,
        twtk_window_widget_create(widget),
        "blue"
    );
}

static void _create_green_box(twtk_widget_t *parent)
{
    twtk_widget_t *widget = twtk_text_widget_create(NULL, 0, 200, 100, 100);
    twtk_widget_set_border_color(widget, TWTK_COLOR_GREEN);
    twtk_widget_set_border_width(widget, 1);

    twtk_widget_add_child_unref(
        parent,
        twtk_window_widget_create(widget),
        "green"
    );
}

static void _create_text_1(twtk_widget_t *parent)
{
    twtk_widget_t *widget = twtk_text_widget_create("Salamipizza\nWurstbrot", 500, 50, 300, 100);
    twtk_widget_set_font_name(widget, "Sans");
    twtk_widget_set_font_color(widget, TWTK_COLOR_WHITE);
    twtk_widget_set_font_size(widget, 40);
    twtk_widget_set_border_color(widget, TWTK_COLOR_BLACK);
    twtk_widget_set_border_width(widget, 1);
    twtk_widget_set_background_color(widget, TWTK_COLOR_YELLOW);

    twtk_widget_add_child_unref(
        parent,
        twtk_window_widget_create(widget),
        "text-1"
    );
}

static void _create_debug(twtk_widget_t *parent)
{
    twtk_widget_t *widget = twtk_text_widget_create("+++", 500, 150, 300, 100);
    twtk_widget_set_font_name(widget, "Sans");
    twtk_widget_set_font_color(widget, TWTK_COLOR_BLACK);
    twtk_widget_set_border_color(widget, TWTK_COLOR_WHITE);
    twtk_widget_set_border_width(widget, 1);
    twtk_widget_set_background_color(widget, TWTK_COLOR_YELLOW);
    twtk_debug_widget_install(widget);

    twtk_widget_add_child_unref(
        parent,
        twtk_window_widget_create(widget),
        "debug"
    );
}

static void _create_image_lara(twtk_widget_t *parent)
{
    twtk_widget_t *widget = twtk_image_widget_create("resources/001.jpg", 400, 100, -1, -1);

    twtk_widget_add_child_unref(
        parent,
        twtk_window_widget_create(widget),
        "image-lara"
    );
}

static void _create_image_black(twtk_widget_t *parent)
{
    twtk_widget_t *widget = twtk_image_widget_create("resources/002.jpg", 50, 50, -1, -1);

    twtk_widget_add_child_unref(
        parent,
        twtk_window_widget_create(widget),
        "image-black"
    );
}

static void _create_box2(twtk_widget_t *parent)
{
    twtk_widget_t *box = twtk_text_widget_create(NULL, 300, 500, 500, 500);
    twtk_widget_set_border_color(box, TWTK_COLOR_BLUE);
    twtk_widget_set_border_width(box, 5);
    twtk_widget_set_font_size(box, 24);

    twtk_widget_add_child_unref(
        parent,
        twtk_window_widget_create(box),
        "box2"
    );
}

static void _init_boxes()
{
    twtk_widget_t *root = twtk_platform_get_root();

    twtk_widget_set_background_color(root, TWTK_COLOR_GREY);

    _create_red_box(root);
    _create_blue_box(root);
    _create_green_box(root);
    _create_text_1(root);
    _create_image_lara(root);
    _create_image_black(root);
    _create_box2(root);
    _create_debug(root);
    _create_position(root);
    _create_menu(root, &menu_main);
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
