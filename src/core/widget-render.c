
#define _DEBUG_NAME	"widget-render"

#ifdef ENABLE_DEBUG_WIDGET_RENDER
#define ENABLE_DEBUG
#endif

#include <assert.h>
#include <malloc.h>
#include <errno.h>
#include <math.h>
#include <assert.h>
#include <twtk/threads.h>
#include <twtk/widget.h>
#include <twtk/color.h>
#include <twtk/events.h>
#include <twtk/widget-list.h>
#include <twtk-private/widget.h>
#include <twtk-private/debug.h>
#include <twtk-private/debug-widget.h>
#include <twtk-private/cairo_util.h>

static inline const char *_widget_name(twtk_widget_t *widget)
{
    return (widget ? widget->name : "<NULL>");
}

static inline const char *_widget_parent_name(twtk_widget_t* widget)
{
    if (widget == NULL)
	return "<NULL>";

    if (widget->parent == NULL)
	return "<ROOT>";

    return widget->parent->name;
}

/* clip-out a single frame */
static void _widget_clip_frame(
    twtk_widget_t *parent,
    twtk_widget_t *frame,
    cairo_t *cr)
{
	/* first sub-path: the whole area */
	cairo_new_sub_path(cr);
	_twtk_ut_rect_to_vec(cr, parent->viewport.size);
	cairo_close_path(cr);

	/* second sub-path: the neighbor widget area */
	cairo_new_sub_path(cr);
	cairo_matrix_t w_matrix = _twtk_widget_calc_matrix(parent);
	cairo_set_matrix(cr, &w_matrix);
	_twtk_ut_rect(cr, frame->viewport);
	cairo_close_path(cr);

	/* set the clip */
	cairo_clip(cr);
}

/* clip-out all sub-windows */
static void _widget_clip_subs(
    twtk_widget_t *widget /* NOTNULL */,
    cairo_t *cr)
{
    assert(widget);
    assert(cr);

    cairo_matrix_t save_matrix;
    cairo_get_matrix(cr, &save_matrix);

    cairo_fill_rule_t save_rule = cairo_get_fill_rule(cr);
    cairo_set_fill_rule(cr, CAIRO_FILL_RULE_EVEN_ODD);

    twtk_widget_list_entry_t *walk;
    for (walk = widget->frames.first; (walk != NULL); walk = walk->prev)
    {
	_DEBUG("clipping out sub %s from %s",
	    _widget_name(walk->widget),
	    _widget_name(widget));

	_widget_clip_frame(widget, walk->widget, cr);
    }

    /* restore old settings */
    cairo_set_fill_rule(cr, save_rule);
    cairo_set_matrix(cr, &save_matrix);
}

/* clip-out neighboring windows */
static void _widget_clip_neigh(
    twtk_widget_t *widget /* NOTNULL */,
    cairo_t *cr /* NOTNULL */)
{
    assert(widget);
    assert(cr);

    /* if widget is transparent, dont clip out whats underneath */
    if (!_twtk_ut_color_visible(widget->background_color))
    {
        _DEBUG("widget %s is transparent", widget->name);
        return;
    }

    /* the root window has no parent, thus no neighbours */
    if (widget->frame == NULL)
	return;

    twtk_widget_list_entry_t *walk;

    cairo_matrix_t save_matrix;
    cairo_get_matrix(cr, &save_matrix);

    cairo_fill_rule_t save_rule = cairo_get_fill_rule(cr);
    cairo_set_fill_rule(cr, CAIRO_FILL_RULE_EVEN_ODD);

    /* iterate backwards until we find ourselves ... */
    for (walk = widget->frame->frames.last; (walk != NULL) && (walk->widget != widget); walk = walk->prev)
    {
	_DEBUG("clipping out neigh \"%s\" from \"%s\" [parent: \"%s\"]",
	    _widget_name(walk->widget),
	    _widget_name(widget),
	    _widget_parent_name(widget));

	_widget_clip_frame(widget->frame, walk->widget, cr);
    }

    assert(walk);

    /* restore old settings */
    cairo_set_fill_rule(cr, save_rule);
    cairo_set_matrix(cr, &save_matrix);
}

static void _widget_prepare_frame(
    twtk_widget_t *widget,
    cairo_t *cr)
{
    _widget_clip_neigh(widget, cr);

    cairo_matrix_t matrix = _twtk_widget_calc_matrix(widget);
    cairo_set_matrix(cr, &matrix);
    _twtk_ut_rect_to_vec(cr, widget->viewport.size);
    cairo_clip(cr);
}

/* fill with background color */
static void _widget_fill_background(twtk_widget_t *widget, cairo_t *cr)
{
    if (!widget->background_color.alpha)
	return;

    _twtk_ut_set_rgba(cr, widget->background_color);
    _twtk_ut_rect_to_vec(cr, widget->viewport.size);

    cairo_fill (cr);
}

/* call the widget's paint() method */
static void _widget_paint(twtk_widget_t *widget, cairo_t *cr)
{
    if ((widget->cls == NULL) || (widget->cls->op_paint == NULL))
        return;

    cairo_save(cr);
    widget->cls->op_paint(widget, cr);
    cairo_restore(cr);
}

/* compose widget onto parent's context */
static void _widget_compose(twtk_widget_t *widget, cairo_t *cr)
{
    _DEBUG("composing \"%s\" onto \"%s\"",
	widget->name,
	(widget->parent ? widget->parent->name : "<NONE>"));

    assert(cr);
    assert(widget);
    assert(widget->paint_cache);

    cairo_save(cr);
    _widget_prepare_frame(widget, cr);
    cairo_set_source (cr, widget->paint_cache);
    cairo_paint (cr);
    cairo_restore(cr);
}

static int _widget_render(twtk_widget_t *widget, cairo_t *cr)
{
    assert(widget);

    int ret = widget->flags & TWTK_WIDGET_FLAG_DIRTY;

    // trigger repaint of sub's
    {
	twtk_widget_list_entry_t *ent;
	for (ent = widget->frames.first; ent != NULL; ent = ent->next)
	    ret |= _widget_render(ent->widget, cr);
    }

    if (ret && widget->paint_cache)
    {
	cairo_pattern_destroy(widget->paint_cache);
	widget->paint_cache = NULL;
    }

    if (widget->paint_cache != NULL)
	return 0;

    _DEBUG("need to render: %s", widget->name);

    /* we'll have to re-render the widget */
    cairo_push_group (cr);

    cairo_save(cr);
    _widget_clip_subs(widget, cr);
    _widget_fill_background(widget, cr);
    _widget_paint(widget, cr);
    cairo_restore(cr);

    /* paint the childs on their given positions */
    {
	twtk_widget_list_entry_t *ent;
	for (ent = widget->frames.first; ent != NULL; ent = ent->next)
	    _widget_compose(ent->widget, cr);
    }

    widget->flags &= ~TWTK_WIDGET_FLAG_DIRTY;
    widget->paint_cache = cairo_pop_group (cr);

    assert(widget->paint_cache);

    return ret;
}

// FIXME: redraw should be made asynchronous
int twtk_widget_do_draw(twtk_widget_t *widget, cairo_t* cr)
{
    if (widget == NULL)
	return -EFAULT;

    /* render the widget and all its subs */
    _DEBUG("recursive render: %s", widget->name);
    _widget_render(widget, cr);

    /* now paint the whole image */
    _DEBUG("final compose: %s", widget->name);
    _widget_compose(widget, cr);

    return 0;
}
