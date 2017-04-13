
#define _GNU_SOURCE
#define _POSIX_C_SOURCE 199309

#include <assert.h>
#include <stdio.h>
#include <cairo.h>
#include <math.h>
#include <twtk/widget.h>
#include <twtk/platform.h>
#include <twtk/color.h>
#include <twtk/events.h>
#include <twtk/widgets/button.h>
#include <twtk/widgets/button.h>
#include <twtk/widgets/image.h>
#include <twtk/widgets/text.h>
#include <twtk/widgets/position.h>
#include <twtk/widgets/movebox.h>
#include <twtk/widgets/window.h>
#include <twtk-private/debug-widget.h>
#include <twtk-private/strutil.h>

#include <signal.h>
#include <time.h>


#define TIMER_SIG	SIGRTMIN

timer_t timerid;
int step = 0;
twtk_widget_t *receiver = NULL;


/** command receiver **/
static int _op_event(twtk_widget_t *widget, twtk_event_t *event, twtk_event_dispatch_t d)
{
    assert(widget);
    assert(event);

    printf("non-mouse event: %s\n", twtk_event_type_name(event->type));

    return 0;
}

static const twtk_widget_class_t _receiver_class_inf = {
    .magic        = 0,
    .op_event     = _op_event,
    .priv_size    = 0
};

/** tick handler for switching the colors **/
static void tick_handler(union sigval sv)
{
    twtk_color_t c;
    switch (++step) {
        case 1:  c = TWTK_COLOR_GREEN;             break;
        case 2:  c = TWTK_COLOR_RED;               break;
        case 3:  c = TWTK_COLOR_BLUE;              break;
        case 4:  c = TWTK_COLOR_YELLOW;            break;
        default: c = TWTK_COLOR_GREY;    step = 0; break;
    }

    twtk_widget_set_background_color(twtk_platform_get_root(), c);
    twtk_platform_redraw();
}

static void setup_timer()
{
    struct sigevent sev = {
        .sigev_notify = SIGEV_THREAD,
        .sigev_value.sival_ptr = &timerid,
        .sigev_notify_function = tick_handler
    };

    struct itimerspec its = {
        .it_value.tv_sec     = 1,
        .it_value.tv_nsec    = 0,
        .it_interval.tv_sec  = 1,
        .it_interval.tv_nsec = 0

    };

    timer_create(CLOCK_REALTIME, &sev, &timerid);
    timer_settime(timerid, 0, &its, NULL);
}

static void _create_position(twtk_widget_t* parent)
{
    return twtk_widget_add_child_unref(
        parent,
        twtk_movebox_widget_create(twtk_position_widget_create(100, 50)),
        "position"
    );
}

static void _create_debug(twtk_widget_t *parent)
{
    twtk_widget_t *widget = twtk_text_widget_create("+++", 500, 50, 300, 100);
    twtk_widget_set_font_name(widget, "Sans");
    twtk_widget_set_font_color(widget, TWTK_COLOR_BLACK);
    twtk_widget_set_border_color(widget, TWTK_COLOR_WHITE);
    twtk_widget_set_border_width(widget, 1);
    twtk_widget_set_background_color(widget, TWTK_COLOR_YELLOW);
    twtk_debug_widget_install(widget);

    twtk_widget_add_child_unref(
        parent,
        twtk_movebox_widget_create(widget),
        "debug"
    );
}

static void _create_image(twtk_widget_t *parent, twtk_dim_t x, twtk_dim_t y, const char* id, const char *fn)
{
    assert(fn);
    assert(id);
    assert(parent);

    twtk_widget_t *w = twtk_image_widget_create(fn, twtk_rect_by_pos(x, y));
    twtk_widget_add_child_unref(
        parent,
        twtk_movebox_widget_create(w),
        id
    );
}

static void _create_btn(twtk_widget_t *parent, twtk_dim_t x, twtk_dim_t y, const char* id, const char *fn)
{
    assert(fn);
    assert(id);
    assert(parent);

    twtk_widget_t *w = twtk_button_widget_create(fn, twtk_rect_by_pos(x, y), "sig1");
    twtk_widget_add_child_unref(parent, w, id);
}

#define BTN_SZ	80

static void _init_widgets()
{
    twtk_widget_t *root = twtk_platform_get_root();
    twtk_vector_t ssz = twtk_widget_get_vsize(root);

    twtk_widget_set_background_color(root, TWTK_COLOR_GREY);

    // create the command receiver
    receiver = twtk_widget_alloc(&_receiver_class_inf);
    twtk_widget_set_name(receiver, "receiver");

    // penguin photo
    _create_image(root, 100,  25, "image-001", "resources/001.jpg");

    // pengiuin comic
    _create_image(root,  10, 100, "image-002", "resources/002.jpg");

    _create_debug(root);
    _create_position(root);

    _create_btn(root, 0,            0,            "btn-top-left",     "resources/button-cross-80x80.png");
    _create_btn(root, ssz.x-BTN_SZ, 0,            "btn-top-right",    "resources/button-cross-80x80.png");
    _create_btn(root, 0,            ssz.y-BTN_SZ, "btn-buttom-left",  "resources/button-cross-80x80.png");
    _create_btn(root, ssz.x-BTN_SZ, ssz.y-BTN_SZ, "btn-buttom-right", "resources/button-cross-80x80.png");
}

int main(void)
{
    twtk_platform_setup();

    _init_widgets();

    setup_timer();

    twtk_platform_redraw();
    twtk_platform_loop();
    twtk_platform_finish();

    return 0;
}
