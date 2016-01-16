
#define _DEBUG_NAME	"widget"

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
#include <twtk-private/strutil.h>
#include <twtk-private/debug.h>
#include <twtk-private/debug-widget.h>
#include <twtk-private/widget.h>
#include <twtk-private/cairo_util.h>

// FIXME: use an allocation cache
twtk_widget_t *twtk_widget_alloc(twtk_widget_class_t *cls)
{
    twtk_widget_t *widget = calloc(1, sizeof(twtk_widget_t));

    TWTK_INIT_LOCK(widget);

    widget->cls = cls;
    if (cls->priv_size)
	widget->priv = calloc(1, cls->priv_size);
    widget->refcnt = 1;
    widget->flags = TWTK_WIDGET_FLAG_DIRTY;
    twtk_widget_list_init(&widget->childs);
    twtk_widget_list_init(&widget->frames);
    return widget;
}

int twtk_widget_set_str(twtk_widget_t *widget, const char *name, const char *value)
{
    if ((widget == NULL) || (name == NULL))
	return -EFAULT;

    if (widget->cls->op_set_str == NULL)
	return -EOPNOTSUPP;

    TWTK_LOCK(widget);
    int ret = widget->cls->op_set_str(widget, name, value);
    TWTK_UNLOCK(widget);

    return ret;
}

int twtk_widget_set_uint(twtk_widget_t *widget, const char *name, uint32_t value)
{
    if ((widget == NULL) || (name == NULL))
	return -EFAULT;

    if (widget->cls->op_set_uint == NULL)
	return -EOPNOTSUPP;

    TWTK_LOCK(widget);
    int ret = widget->cls->op_set_uint(widget, name, value);
    TWTK_UNLOCK(widget);

    return ret;
}

int twtk_widget_set_float(twtk_widget_t *widget, const char *name, double value)
{
    if ((widget == NULL) || (name == NULL))
	return -EFAULT;

    if (widget->cls->op_set_float == NULL)
	return -EOPNOTSUPP;

    TWTK_LOCK(widget);
    int ret = widget->cls->op_set_float(widget, name, value);
    TWTK_UNLOCK(widget);

    return ret;
}

int twtk_widget_set_color(twtk_widget_t *widget, const char *name, twtk_color_t color)
{
    if ((widget == NULL) || (name == NULL))
	return -EFAULT;

    if (widget->cls->op_set_color == NULL)
	return -EOPNOTSUPP;

    TWTK_LOCK(widget);
    int ret = widget->cls->op_set_color(widget, name, color);
    TWTK_UNLOCK(widget);

    return ret;
}

void twtk_widget_add_child(twtk_widget_t *parent, twtk_widget_t *child, const char *name)
{
    // FIXME! this are different hierarchies
    // but we yet dont have the neccessary platform callbacks which would handle things
    // like automatically adding windows/frames around widgets, etc
    assert(parent);
    assert(child);
    assert(child->parent == NULL);
    assert(child->frame == NULL);
    twtk_widget_list_add(&parent->childs, child, name);

    /* add the child to a frame, so it becomes actually visible */
    TWTK_LOCK(parent);
    TWTK_LOCK(child);

    child->parent = twtk_widget_ref(parent);

    /* FIXME: handle popups */

    if (parent->cls->op_insert_child)
    {
	/* class has it's own insert method */
	parent->cls->op_insert_child(parent, child, name);
    }
    else
    {
	/* default behaviour: add the child into the parent's frame */
	twtk_widget_list_add(&parent->frames, child, name);
	child->frame = twtk_widget_ref(parent);
    }

    twtk_widget_dirty(parent);
    twtk_widget_dirty(child);

    TWTK_UNLOCK(child);
    TWTK_UNLOCK(parent);
}

void twtk_widget_add_child_unref(twtk_widget_t *parent, twtk_widget_t *child, const char *name)
{
    twtk_widget_add_child(parent, child, name);
    twtk_widget_unref(child);
}

void twtk_widget_destroy(twtk_widget_t *widget)
{
    assert(widget);

    _DEBUG("freeing widget (class: %s)", widget->cls->name);

    twtk_widget_list_fini (&widget->frames);
    twtk_widget_list_fini (&widget->childs);

    if (widget->cls->op_fini)
	widget->cls->op_fini(widget);

    if (widget->priv)
	free(widget->priv);

    TWTK_DESTROY_LOCK(widget);

    free(widget);
}

int twtk_widget_dispatch(twtk_widget_t *parent, twtk_event_t *event)
{
    /* fixme: we yet have to decide, under which circumstances messages
       are passed to parent vs. child - maybe add some flags ? */
    int ret = twtk_widget_event(parent, event);

    if (ret & TWTK_EVSTAT_CONSUMED)
	return ret;

    if (event->type == TWTK_EVENT_MOUSE)
    {
	twtk_widget_t *child;
	twtk_vector_t pos;
	char *name = NULL;

	if (!twtk_widget_list_find_pos(
		&(parent->frames),
		event->mouse.absolute,
		&child,
		&pos,
		&name))
	    return ret;

	twtk_event_t ev = *event;
	ev.mouse.absolute = pos;

	char buffer[1024];
	snprintf(buffer, sizeof(buffer), "[%4.0f : %4.0f] %s", pos.x, pos.y, name);
	twtk_debug_widget_set_text(buffer);

	/* pass to childs */
	ret |= twtk_widget_dispatch(child, &ev);
	twtk_widget_unref(child);
	return ret;
    }

    /* nothing to do */
    return 0;
}

int twtk_widget_set_name(twtk_widget_t *widget, const char *name)
{
    STRBUF_SET(widget->name, name);
    return 1;
}

int twtk_widget_set_border_color(twtk_widget_t *widget, twtk_color_t color)
{
    return twtk_widget_set_color(widget, "border-color", color);
}

int twtk_widget_set_background_color(twtk_widget_t *widget, twtk_color_t color)
{
    // FIXME: not locking right now - risk is relatively low
    widget->background_color = color;
    twtk_widget_dirty(widget);
    return 1;
}

int twtk_widget_set_font_color(twtk_widget_t *widget, twtk_color_t color)
{
    return twtk_widget_set_color(widget, "font-color", color);
}

int twtk_widget_set_font_size(twtk_widget_t *widget, double size)
{
    return twtk_widget_set_float(widget, "font-size", size);
}

int twtk_widget_set_border_width(twtk_widget_t *widget, double size)
{
    return twtk_widget_set_float(widget, "border-width", size);
}

int twtk_widget_set_font_name(twtk_widget_t *widget, const char* n)
{
    return twtk_widget_set_str(widget, "font-name", n);
}

int twtk_widget_set_text(twtk_widget_t *widget, const char* text)
{
    return twtk_widget_set_str(widget, "text", text);
}

void twtk_widget_move(twtk_widget_t *widget, double x, double y)
{
    assert(widget);
    widget->viewport_pos = TWTK_VECTOR(x, y);
    twtk_widget_dirty(widget);
}

void twtk_widget_move_rel(twtk_widget_t *widget, double x, double y)
{
    assert(widget);
    widget->viewport_pos = twtk_vector_add(widget->viewport_pos, TWTK_VECTOR(x, y));
    twtk_widget_dirty(widget);
}

void twtk_widget_resize(twtk_widget_t *widget, double w, double h)
{
    assert(widget);
    widget->viewport_size = TWTK_VECTOR(w, h);
    twtk_widget_dirty(widget);
}

void twtk_widget_rotate(twtk_widget_t *widget, double a)
{
    assert(widget);
    widget->viewport_angle = M_PI/180*a;
    twtk_widget_dirty(widget);
}

void twtk_widget_vresize(twtk_widget_t *widget, double w, double h)
{
    assert(widget);
    widget->surface_size = TWTK_VECTOR(w, h);
    twtk_widget_dirty(widget);
}

void twtk_widget_dirty(twtk_widget_t *widget)
{
    assert(widget);
    // FIXME: atomic operations ?
    widget->flags |= TWTK_WIDGET_FLAG_DIRTY;
}

int twtk_widget_event(twtk_widget_t *widget, twtk_event_t *event)
{
    if (widget->cls->op_event == NULL)  // ignored / pass to childs
        return 0;

    return widget->cls->op_event(widget, event);
}
