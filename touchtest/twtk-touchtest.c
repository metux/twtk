
#define _GNU_SOURCE
#define _POSIX_C_SOURCE 199309

#include <assert.h>
#include <stdio.h>
#include <twtk/widget.h>
#include <twtk/platform.h>
#include <twtk/color.h>
#include <twtk/events.h>
#include <twtk/widgets/button.h>
#include <twtk/widgets/position.h>

#define NUM_BUTTONS	5
#define WALK_BUTTONS	4

#define BTN_SZ		80
#define BTN_ABORT_SZ	160

#define ID_TOP_LEFT	0
#define ID_TOP_RIGHT	1
#define ID_BOTTOM_RIGHT	2
#define ID_BOTTOM_LEFT	3
#define ID_ABORT	4

typedef struct {
    const char    *name;
    const char    *img1;
    const char    *img2;
    twtk_rect_t   rect;
    twtk_widget_t *widget;
} btninf_t;

btninf_t button_inf[NUM_BUTTONS] = {
    {
        .name = "btn-top-left",
        .img1 = "button-1-red-80x80.png",
        .img2 = "button-1-green-80x80.png"
    },
    {
        .name = "btn-top-right",
        .img1 = "button-2-red-80x80.png",
        .img2 = "button-2-green-80x80.png"
    },
    {
        .name = "btn-bottom-right",
        .img1 = "button-3-red-80x80.png",
        .img2 = "button-3-green-80x80.png"
    },
    {
        .name = "btn-bottom-left",
        .img1 = "button-4-red-80x80.png",
        .img2 = "button-4-green-80x80.png"
    },
    {
        .name = "btn-abort",
        .img1 = "button-abort.png",
        .img2 = "button-okay.png"
    }
};

twtk_widget_t *receiver = NULL;
int btn_count = 0;

static twtk_widget_t* make_button(int idx, char dn)
{
    char buffer[4096];
    const char* p = getenv("TWTK_PIXMAP_DIR");
    snprintf(buffer, sizeof(buffer), "%s/%s",
        (p ? p : PIXMAP_DIR),
        (dn ? button_inf[idx].img2 : button_inf[idx].img1));

    twtk_widget_t *w = twtk_button_widget_create(
        buffer,
        button_inf[idx].rect,
        button_inf[idx].name // using btn name as signal id
    );

    twtk_widget_set_border_width(w, 20);
    twtk_widget_set_border_color(w, TWTK_COLOR_BLUE);
    twtk_widget_add_child(twtk_platform_get_root(), w, button_inf[idx].name);

    return w;
}

static void _create_btn(int idx)
{
    twtk_widget_t *w = make_button(idx, 0);
    twtk_widget_set_controller(w, receiver);
    button_inf[idx].widget = w;
}

static void _switch_btn(int idx)
{
    twtk_widget_remove_child(NULL, button_inf[idx].widget);
    twtk_widget_unref(button_inf[idx].widget);
    button_inf[idx].widget = make_button(idx, 1);

    if (idx == ID_ABORT)
         twtk_widget_set_controller(button_inf[idx].widget, receiver);
}

/** command receiver **/
static int _recv_event(twtk_widget_t *widget, twtk_event_t *event, twtk_event_dispatch_t d)
{
    assert(widget);
    assert(event);

    if (event->type == TWTK_EVENT_ACTION) {
        int x;
        for (x=0; x<NUM_BUTTONS; x++) {
            if (strcmp(event->action.signal, button_inf[x].name)==0) {
                if (x == btn_count) {
                    _switch_btn(x);
                    btn_count++;
                }
            }
        }

        if (btn_count == WALK_BUTTONS)
            _switch_btn(ID_ABORT);

        if (strcmp(event->action.signal, button_inf[ID_ABORT].name)==0) {
            if (btn_count >= WALK_BUTTONS) {
                printf("OK\n");
                exit(0);
            } else {
                printf("FAILED\n");
                exit(1);
            }
        }
    }

    return 0;
}

static const twtk_widget_class_t _receiver_class_inf = {
    .op_event     = _recv_event,
};

static void _init_btnpos()
{
    twtk_vector_t ssz = twtk_widget_get_vsize(twtk_platform_get_root());

    button_inf[ID_TOP_LEFT].rect     = twtk_rect_by_pos(0,            0);
    button_inf[ID_TOP_RIGHT].rect    = twtk_rect_by_pos(ssz.x-BTN_SZ, 0);
    button_inf[ID_BOTTOM_RIGHT].rect = twtk_rect_by_pos(ssz.x-BTN_SZ, ssz.y-BTN_SZ);
    button_inf[ID_BOTTOM_LEFT].rect  = twtk_rect_by_pos(0,            ssz.y-BTN_SZ);

    button_inf[ID_ABORT].rect        = twtk_rect_by_pos((ssz.x - BTN_ABORT_SZ)/2, (ssz.y - BTN_ABORT_SZ)/2);
}

static void _init_widgets()
{
    twtk_widget_t *root = twtk_platform_get_root();
    twtk_vector_t ssz = twtk_widget_get_vsize(root);

    _init_btnpos();

    // create the command receiver
    receiver = twtk_widget_alloc(&_receiver_class_inf);
    twtk_widget_set_name(receiver, "receiver");

    // create position widget
    twtk_widget_add_child_unref(
        root,
        twtk_position_widget_create(100, 50),
        "position"
    );

    int x;
    for (x=0; x<NUM_BUTTONS; x++)
        _create_btn(x);
}

int main(void)
{
    twtk_platform_setup();

    _init_widgets();

    twtk_platform_redraw();
    twtk_platform_loop();
    twtk_platform_finish();

    return 0;
}
