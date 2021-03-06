
#ifdef ENABLE_DEBUG_PLATFORM_GENERIC
#define ENABLE_DEBUG
#endif

#define _GNU_SOURCE

#define _DEBUG_NAME	"platform-generic"

#include <assert.h>
#include <time.h>
#include <cairo.h>
#include <cairo-drm.h>
#include <twtk/types.h>
#include <twtk/widget-helper.h>
#include <twtk/platform.h>
#include <twtk/widget.h>
#include <twtk/viewport.h>
#include <twtk-private/debug.h>
#include <twtk-private/platform-generic.h>
#include <twtk-private/cairo_util.h>
#include <twtk/color.h>


cairo_t *_twtk_platform_generic_get_context(twtk_platform_t *pl)
{
    assert(pl);
    return cairo_create(pl->surface);
}

cairo_t *_twtk_platform_generic_get_temp_context(twtk_platform_t *pl)
{
    assert(pl);
    return cairo_create(pl->temp_surface);
}

int _twtk_platform_generic_free_context(twtk_platform_t *pl, cairo_t *ctx)
{
    assert(pl);
    assert(ctx);
    cairo_destroy(ctx);
    return 1;
}

int _twtk_platform_generic_free_temp_context(twtk_platform_t *pl, cairo_t *ctx)
{
    assert(pl);
    assert(ctx);
    cairo_destroy(ctx);
    return 1;
}

#define MAGIC			0x4f52544f30303030

#define _WIDGET_PRIV_TYPE	_root_priv_t
#define _WIDGET_CLASS_INF	_root_class_inf

static int _op_invalidate_rect(twtk_widget_t *widget,
                               twtk_rect_t rect,
                               cairo_matrix_t *client_matrix)
    __attribute__((nonnull(1,3)));

typedef struct
{
    twtk_platform_t *platform;
} _root_priv_t;

static twtk_widget_class_t _root_class_inf = {
    .magic     = MAGIC,
    .priv_size = sizeof(_root_priv_t),
    .op_invalidate_rect = _op_invalidate_rect
};

static twtk_widget_t *_create_root(twtk_platform_t *platform, cairo_surface_t *surface)
{
    assert(surface);
    assert(platform);

    twtk_widget_t *r = twtk_widget_alloc(&_root_class_inf);
    assert(r);
    assert(r->priv);

    _root_priv_t *priv = (_root_priv_t*)r->priv;
    priv->platform = platform;

    /* retrieve surface geometry */
    {
        cairo_t *ctx = cairo_create(surface);
        cairo_clip_extents (ctx,
            &r->viewport.pos.x,
            &r->viewport.pos.y,
            &r->viewport.size.x,
            &r->viewport.size.y
        );
    }

    r->surface = r->viewport;

    /** fixme: we should add surface-type independent functions in cairo **/

    twtk_widget_set_name(r, "<ROOT>");
    twtk_widget_set_background_color(r, TWTK_COLOR_BLACK);

    return r;
}

twtk_widget_t *_twtk_platform_generic_get_root(twtk_platform_t *platform)
{
    assert(platform);

    if (platform->root == NULL)
    {
        platform->root = _create_root(platform, platform->surface);
        twtk_widget_dirty(platform->root);
        platform->op_redraw(platform);
    }

    return platform->root;
}

static twtk_widget_t *_translate_to_root(
    twtk_widget_t *widget,
    twtk_vector_t pos,
    twtk_vector_t *ret_pos)
{
    if (widget->frame == NULL)
    {
        _DEBUG("reached the top: %.0f:%.0f", pos.x, pos.y);
        *ret_pos = pos;
        return widget;
    }

    twtk_vector_t p2;
    twtk_viewport_pos_to_frame(widget, pos, &p2);

    _DEBUG("translated to parent: %0.f:%0.f", p2.x, p2.y);
    return _translate_to_root(widget->parent, p2, ret_pos);
}

int _twtk_platform_generic_map_widget(twtk_platform_t *platform, twtk_widget_t *widget, twtk_widget_t *parent)
{
    assert(platform);
    assert(widget);

    if (parent == NULL)
        return twtk_widget_set_frame(widget, NULL);

    if (widget->flags & TWTK_WIDGET_FLAG_POPUP)
    {
        twtk_vector_t newpos;

        _DEBUG("popup window %0.f:%0.f", widget->viewport.pos.x, widget->viewport.pos.y);
        twtk_widget_t *newparent = _translate_to_root(parent, widget->viewport.pos, &newpos);
        widget->viewport.pos = newpos;

        _DEBUG("  translated: %0.f:%0.f", newpos.x, newpos.y);
        twtk_widget_set_frame(widget, newparent);
    }
    else
    {
        twtk_widget_set_frame(widget, parent);
    }

    return 0;
}

static void _clip_dirty(twtk_platform_t *platform, cairo_t *cr)
{
    if (platform->regions == NULL)
    {
        _DEBUG("no invalidated regions to clip");
        return;
    }

    cairo_matrix_t old_matrix;
    cairo_get_matrix(cr, &old_matrix);

    _twtk_platform_generic_region_t* regions = platform->regions;

    while (regions)
    {
        _twtk_platform_generic_region_t* r = regions;
        regions = regions->next;

        _DEBUG("clipping to invalidated region: %0.2f:%0.2f / %0.2f:%0.2f",
            r->rect.pos.x, r->rect.pos.y, r->rect.size.x, r->rect.size.y);

        cairo_set_matrix(cr, &r->matrix);
        _twtk_ut_rect(cr, r->rect);
    }

    cairo_clip(cr);
    cairo_set_matrix(cr, &old_matrix);
}

static void _clean_dirty(twtk_platform_t *platform)
{
    if (platform->regions == NULL)
        return;

    _twtk_platform_generic_region_t* regions = platform->regions;
    platform->regions = NULL;

    while (regions)
    {
        _twtk_platform_generic_region_t* r = regions;
        regions = regions->next;
        free(r);
    }
}

int _twtk_platform_generic_flush(twtk_platform_t *platform)
{
    return 0;
}

static int _render_root(twtk_widget_t *widget, cairo_t *target_cr, twtk_platform_t *platform)
{
    assert(widget);

    int ret = widget->flags & TWTK_WIDGET_FLAG_DIRTY;

    cairo_t *cr = platform->op_get_temp_context(platform);

    struct timespec start;
    clock_gettime(CLOCK_MONOTONIC, &start);

    // trigger repaint of sub's
    {
        twtk_widget_list_entry_t *ent;
        for (ent = widget->frames.first; ent != NULL; ent = ent->next)
            ret |= twtk_widget_render(ent->widget, cr);
    }

    struct timespec after_subrender;
    clock_gettime(CLOCK_MONOTONIC, &after_subrender);

    cairo_pattern_destroy(widget->paint_cache);
    widget->paint_cache = NULL;

    cairo_push_group (cr);

    /* clip to the regions that actually changed */
    _clip_dirty(platform, cr);

    _twtk_ut_set_rgba(cr, widget->background_color);
    _twtk_ut_rect_to_vec(cr, widget->viewport.size);

    cairo_fill (cr);

    _clip_dirty(platform, cr);
    /* paint the childs on their given positions */
    {
        twtk_widget_list_entry_t *ent;
        for (ent = widget->frames.first; ent != NULL; ent = ent->next)
        {
            _DEBUG("composing \"%s\" onto \"%s\"",
                ent->widget->name,
                (ent->widget->parent ? ent->widget->parent->name : "<NONE>"));

            assert(ent->widget);
            assert(ent->widget->paint_cache);

            cairo_save(cr);
            twtk_widget_render_prepare_frame(ent->widget, cr);
            cairo_set_source (cr, ent->widget->paint_cache);
            cairo_paint (cr);
            cairo_restore(cr);
        }
    }

    widget->flags &= ~TWTK_WIDGET_FLAG_DIRTY;
    widget->paint_cache = cairo_pop_group (cr);

    platform->op_free_temp_context(platform, cr);

    struct timespec after_rootrender;
    clock_gettime(CLOCK_MONOTONIC, &after_rootrender);

    /* clip to the regions that actually changed */
    _clip_dirty(platform, target_cr);

    /* now do the final composition onto screen */
    cairo_set_source(target_cr, widget->paint_cache);
    cairo_paint(target_cr);

    struct timespec after_composite;
    clock_gettime(CLOCK_MONOTONIC, &after_composite);

    _DEBUG("timing: subrender=%8ld us - rootrender=%8ld us - comp=%8ld us\n",
        (after_subrender.tv_nsec - start.tv_nsec)/1000,
        (after_rootrender.tv_nsec - after_subrender.tv_nsec)/1000,
        (after_composite.tv_nsec - after_rootrender.tv_nsec)/1000);

    _clean_dirty(platform);

    return ret;
}

int _twtk_platform_generic_redraw(twtk_platform_t *platform)
{
    assert(platform);

    pthread_mutex_lock(&platform->redraw_lock);

    twtk_widget_t *root = platform->op_get_root(platform);
    assert(root);

    if (!(root->flags & TWTK_WIDGET_FLAG_DIRTY))
        goto out;

    cairo_t *cr = _twtk_current_platform->op_get_context(platform);
    assert(cr);

    /* render the root widget and all it's children */
    _DEBUG("rendering root: %s", root->name);
    _render_root(root, cr, platform);

    platform->op_free_context(platform, cr);

out:
    pthread_mutex_unlock(&platform->redraw_lock);
    return platform->op_flush(platform);
}

int _twtk_platform_generic_init(twtk_platform_t *platform)
{
    assert(platform);

    _twtk_lock_init(&platform->redraw_lock);

    platform->op_get_root     = _twtk_platform_generic_get_root;
    platform->op_get_context  = _twtk_platform_generic_get_context;
    platform->op_get_temp_context  = _twtk_platform_generic_get_temp_context;
    platform->op_free_context = _twtk_platform_generic_free_context;
    platform->op_free_temp_context = _twtk_platform_generic_free_context;
    platform->op_map_widget   = _twtk_platform_generic_map_widget;
    platform->op_redraw       = _twtk_platform_generic_redraw;
    platform->op_flush        = _twtk_platform_generic_flush;

    return 0;
}

static int _op_invalidate_rect(twtk_widget_t *widget, twtk_rect_t rect, cairo_matrix_t *client_matrix)
{
    assert(widget);
    assert(client_matrix);
    assert(widget->frame == NULL);
    assert(widget->priv);

    widget->flags |= TWTK_WIDGET_FLAG_DIRTY;

    _root_priv_t *priv = (_root_priv_t*)widget->priv;

    _DEBUG("invalidate_rect: (ROOT)  widget=%10s rect=%4.0f:%4.0f:%4.0f:%4.0f/%4.0f",
        widget->name, rect.pos.x, rect.pos.y, rect.size.x, rect.size.y, rect.angle);

    _twtk_platform_generic_region_t *region = calloc(1,sizeof(_twtk_platform_generic_region_t));
    region->matrix = *client_matrix;
    region->rect = rect;
    region->next = priv->platform->regions;
    priv->platform->regions = region;

    return 1;
}
