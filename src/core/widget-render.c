
#define _DEBUG_NAME	"widget-render"

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

static void _widget_prepare_frame(twtk_widget_t *widget, cairo_t *cr)
{
    cairo_matrix_t matrix = _twtk_widget_calc_matrix(widget);
    cairo_set_matrix(cr, &matrix);
    cairo_rectangle(cr, 0, 0, widget->viewport_size.x, widget->viewport_size.y);
    cairo_clip(cr);
}

/* fill with background color */
static void _widget_fill_background(twtk_widget_t *widget, cairo_t *cr)
{
    if (!widget->background_color.alpha)
	return;

    _twtk_ut_set_rgba(cr, widget->background_color);
    cairo_rectangle (cr, 0, 0, widget->viewport_size.x, widget->viewport_size.y);
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
static void _widget_compose(twtk_widget_t *widget, cairo_t *cr, const char *parent_name, const char *child_name)
{
    _DEBUG("composing %s onto %s", child_name, parent_name);

    assert(cr);
    assert(widget);
    assert(widget->paint_cache);

    cairo_save(cr);
    _widget_prepare_frame(widget, cr);
    cairo_set_source (cr, widget->paint_cache);
    cairo_paint (cr);
    cairo_restore(cr);
}

static int _widget_render(twtk_widget_t *widget, cairo_t *cr, const char* name)
{
    assert(widget);

    int ret = widget->flags & TWTK_WIDGET_FLAG_DIRTY;

    // trigger repaint of sub's
    {
	twtk_widget_list_entry_t *ent;
	for (ent = widget->frames.first; ent != NULL; ent = ent->next)
	    ret |= _widget_render(ent->widget, cr, ent->name);
    }

    if (ret && widget->paint_cache)
    {
	cairo_pattern_destroy(widget->paint_cache);
	widget->paint_cache = NULL;
    }

    if (widget->paint_cache != NULL)
	return 0;

    _DEBUG("need to render: %s", name);

    /* we'll have to re-render the widget */
    cairo_push_group (cr);

    _widget_fill_background(widget, cr);
    _widget_paint(widget, cr);

    /* paint the childs on their given positions */
    {
	twtk_widget_list_entry_t *ent;
	for (ent = widget->frames.first; ent != NULL; ent = ent->next)
	    _widget_compose(ent->widget, cr, name, ent->name);
    }

    widget->flags &= ~TWTK_WIDGET_FLAG_DIRTY;
    widget->paint_cache = cairo_pop_group (cr);

    assert(widget->paint_cache);

    return ret;
}

// FIXME: redraw should be made asynchronous
int twtk_widget_do_draw(twtk_widget_t *widget, cairo_t* cr, const char *name)
{
    if (widget == NULL)
	return -EFAULT;

    /* render the widget and all its subs */
    _DEBUG("recursive render: %s", name);
    _widget_render(widget, cr, name);

    /* now paint the whole image */
    _DEBUG("final compose: %s", name);
    _widget_compose(widget, cr, "<root>", name);

    return 0;
}
