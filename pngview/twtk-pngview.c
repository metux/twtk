
#define _GNU_SOURCE
#define _POSIX_C_SOURCE 199309

#include <assert.h>
#include <stdio.h>
#include <twtk/widget.h>
#include <twtk/platform.h>
#include <twtk/widgets/image.h>


static void _create_image(twtk_widget_t *parent, const char* id, const char *fn)
{
    assert(fn);
    assert(id);
    assert(parent);

    twtk_widget_t *w = twtk_image_widget_create(fn, twtk_rect_by_pos(0, 0));
    twtk_widget_add_child_unref(parent, w, id);
}

int main(int argc, char* argv[])
{
    if (argc < 2) {
        fprintf(stderr, "%s <png-file>\n", argv[0]);
        exit(1);
    }

    twtk_platform_setup();

    twtk_widget_t *root = twtk_platform_get_root();

    _create_image(root, "img", argv[1]);

    twtk_platform_redraw();
    twtk_platform_loop();
    twtk_platform_finish();

    return 0;
}
