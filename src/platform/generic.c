
#include <assert.h>
#include <cairo.h>
#include <cairo-drm.h>
#include <twtk/types.h>
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

twtk_widget_t *_twtk_platform_generic_get_root(twtk_platform_t *platform)
{
    assert(platform);

    if (platform->root == NULL)
    {
	assert(platform->surface);

	platform->root = twtk_text_widget_create(
	    NULL,
	    0,
	    0,
	    /** fixme: we should add surface-type independent functions in cairo **/
	    cairo_drm_surface_get_width(platform->surface),
	    cairo_drm_surface_get_height(platform->surface));

	twtk_widget_set_background_color(
	    platform->root,
	    (twtk_color_t){.alpha = 0xff, .red = 0x15, .green = 0x22, .blue = 0x33}
	);
    }

    return platform->root;
}
