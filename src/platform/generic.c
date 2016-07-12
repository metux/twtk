
#include <assert.h>
#include <cairo.h>
#include <cairo-drm.h>
#include <twtk/types.h>
#include <twtk/widget-helper.h>
#include <twtk/platform.h>
#include <twtk/widget.h>
#include <twtk-private/platform-generic.h>
#include <twtk/widgets.h>
#include <twtk/color.h>


cairo_t *_twtk_platform_generic_get_context(twtk_platform_t *pl)
{
    assert(pl);
    return cairo_create(pl->surface);
}

int _twtk_platform_generic_free_context(twtk_platform_t *pl, cairo_t *ctx)
{
    assert(pl);
    assert(ctx);
    cairo_destroy(ctx);
    return 1;
}

#define MAGIC			0x4f52544f30303030

#define _WIDGET_PRIV_TYPE	_root_priv_t
#define _WIDGET_CLASS_INF	_root_class_inf

static int _op_event(twtk_widget_t *widget, twtk_event_t *event, twtk_event_dispatch_t d)
    __attribute__((nonnull(1,2)));

typedef struct
{
} _root_priv_t;

static twtk_widget_class_t _root_class_inf = {
    .magic     = MAGIC,
    .priv_size = sizeof(_root_priv_t)
};

static twtk_widget_t *_create_root(cairo_surface_t *surface)
{
    assert(surface);

    twtk_widget_t *r = twtk_widget_alloc(&_root_class_inf);
    assert(r);

    /* retrieve surface geometry */
    {
        cairo_t *ctx = cairo_create(surface);
        double x1, y1, x2, y2;
        cairo_clip_extents (ctx,
            &r->viewport.pos.x,
            &r->viewport.pos.y,
            &r->viewport.size.x,
            &r->viewport.size.y
        );
    }

    /** fixme: we should add surface-type independent functions in cairo **/

    twtk_widget_set_name(r, "<ROOT>");
    twtk_widget_set_background_color(r, TWTK_COLOR_BLACK);

    return r;
}

twtk_widget_t *_twtk_platform_generic_get_root(twtk_platform_t *platform)
{
    assert(platform);

    if (platform->root == NULL)
        platform->root = _create_root(platform->surface);

    return platform->root;
}

int _twtk_platform_generic_map_widget(twtk_platform_t *platform, twtk_widget_t *widget, twtk_widget_t *parent)
{
    assert(platform);
    assert(widget);

    twtk_widget_set_frame(widget, parent);

    return 0;
}
