
#define _DEBUG_NAME	"widget"

#ifdef ENABLE_DEBUG_WIDGET
#define ENABLE_DEBUG
#endif

#define _GNU_SOURCE

#include <assert.h>
#include <malloc.h>
#include <errno.h>
#include <math.h>
#include <assert.h>
#include <twtk/threads.h>
#include <twtk/widget.h>
#include <twtk/viewport.h>
#include <twtk/color.h>
#include <twtk/events.h>
#include <twtk/platform.h>
#include <twtk/widget-list.h>
#include <twtk-private/strutil.h>
#include <twtk-private/debug.h>
#include <twtk-private/debug-widget.h>
#include <twtk-private/cairo_util.h>

// FIXME: use an allocation cache
twtk_widget_t *twtk_widget_alloc(const twtk_widget_class_t *cls)
{
    TWTK_NEW_OBJECT(twtk_widget_t,widget);

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

    if (name)
        twtk_widget_set_name(child, name);

    twtk_widget_set_parent(child, parent);

    /* if the class doesnt have its own insert_child() method, just add
       it into parent frame */
    if (parent->cls->op_insert_child)
        parent->cls->op_insert_child(parent, child);
    else
        twtk_platform_map_widget(child, parent);

    twtk_widget_dirty(parent);
    twtk_widget_dirty(child);
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

static twtk_widget_t *last_mouse_consumer = NULL;

/** update the debug widget **/
static int _update_debug(twtk_widget_t *widget, twtk_event_t *event)
{
    assert(widget);
    assert(event);

    char buffer[1024];

    snprintf(
        buffer,
        sizeof(buffer),
        "[%4.0f : %4.0f] %s",
        event->mouse.pos.x,
        event->mouse.pos.y,
        widget->name);

    twtk_debug_widget_set_text(buffer);

    return 0;
}

/** dispatch LEAVE to the last active frame, if changed **/
static int _dispatch_mouse_last(twtk_widget_t *parent, twtk_event_t *event)
{
    assert(parent);
    assert(event);

    if (parent == last_mouse_consumer)
        return 0;

    if (last_mouse_consumer != NULL)
    {
        twtk_event_t leave_event = { 0 };
        leave_event.type = TWTK_EVENT_MOUSE;
        leave_event.mouse.event = TWTK_EVENT_MOUSE_LEAVE;
        twtk_widget_event(last_mouse_consumer, &leave_event, TWTK_EVENT_DISPATCH_DIRECT);
        twtk_widget_unref(last_mouse_consumer);
    }

    last_mouse_consumer = twtk_widget_ref(parent);

    return 0;
}

static int _dispatch_mouse(twtk_widget_t *parent, twtk_event_t *event)
{
    twtk_widget_t *child;
    twtk_vector_t pos;
    char *name = NULL;

    assert(parent);
    assert(event);

    /** check whether the position points into a subframe - in that case
        pass down there event there
     **/
    if (twtk_widget_list_find_pos(
        &(parent->frames),
        event->mouse.pos,
        &child,
        &pos,
        &name))
    {
        /* allow the event to be seen and optionally consumed by the parent */
        int ret = twtk_widget_event(parent, event, TWTK_EVENT_DISPATCH_BEFORE);

        if (ret & TWTK_EVSTAT_CONSUMED)
        {
            twtk_widget_unref(child);
            return ret;
        }

        /** position points into a subframe - pass it there **/
        twtk_event_t ev = *event;
        ev.mouse.pos = pos;

        /* pass to childs */
        ret = twtk_widget_dispatch(child, &ev);
        twtk_widget_unref(child);

        if (ret & TWTK_EVSTAT_CONSUMED)
            return ret;

        return twtk_widget_event(parent, event, TWTK_EVENT_DISPATCH_AFTER);
    }
    else
    {
        /** position points into current frame - process there **/
        _dispatch_mouse_last(parent, event);
        _update_debug(parent, event);

        return twtk_widget_event(parent, event, TWTK_EVENT_DISPATCH_DIRECT);
    }
}

int twtk_widget_dispatch(twtk_widget_t *parent, twtk_event_t *event)
{
    if (event->type == TWTK_EVENT_MOUSE)
        return _dispatch_mouse(parent, event);

    return twtk_widget_event(parent, event, TWTK_EVENT_DISPATCH_AFTER);
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

int twtk_widget_set_font_size(twtk_widget_t *widget, twtk_dim_t size)
{
    return twtk_widget_set_float(widget, "font-size", size);
}

int twtk_widget_set_border_width(twtk_widget_t *widget, twtk_dim_t size)
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

static void _invalidate_viewport(twtk_widget_t *widget, twtk_rect_t old_viewport)
{
    /* invalidate our old and new region within our frame */
    if (widget->frame == NULL)
        return;

    twtk_widget_invalidate_rect(widget->frame, old_viewport);
    twtk_widget_invalidate_rect(widget->frame, widget->viewport);
}

void twtk_widget_move(twtk_widget_t *widget, twtk_vector_t pos)
{
    assert(widget);
    twtk_rect_t old_viewport = widget->viewport;
    widget->viewport.pos = pos;

    widget->flags &= ~(TWTK_WIDGET_FLAG_HAS_MATRIX | TWTK_WIDGET_FLAG_HAS_INV_MATRIX);

    _invalidate_viewport(widget, old_viewport);
}

void twtk_widget_move_rel(twtk_widget_t *widget, twtk_vector_t vec)
{
    assert(widget);
    twtk_rect_t old_viewport = widget->viewport;
    widget->viewport.pos = twtk_vector_add(widget->viewport.pos, vec);

    widget->flags &= ~(TWTK_WIDGET_FLAG_HAS_MATRIX | TWTK_WIDGET_FLAG_HAS_INV_MATRIX);

    _invalidate_viewport(widget, old_viewport);
}

void twtk_widget_set_viewport(twtk_widget_t *widget, twtk_rect_t vp)
{
    assert(widget);

    twtk_rect_t old_viewport = widget->viewport;
    widget->viewport = vp;

    widget->flags &= ~(TWTK_WIDGET_FLAG_HAS_MATRIX | TWTK_WIDGET_FLAG_HAS_INV_MATRIX);

    _invalidate_viewport(widget, old_viewport);
}

void twtk_widget_resize(twtk_widget_t *widget, twtk_vector_t size)
{
    assert(widget);
    twtk_rect_t old_viewport = widget->viewport;
    widget->viewport.size = size;

    _invalidate_viewport(widget, old_viewport);
}

void twtk_widget_rotate(twtk_widget_t *widget, twtk_dim_t a)
{
    assert(widget);
    twtk_rect_t old_viewport = widget->viewport;
    widget->viewport.angle = M_PI/180*a;

    widget->flags &= ~(TWTK_WIDGET_FLAG_HAS_MATRIX | TWTK_WIDGET_FLAG_HAS_INV_MATRIX);

    _invalidate_viewport(widget, old_viewport);
}

void twtk_widget_vresize(twtk_widget_t *widget, twtk_vector_t size)
{
    assert(widget);
    widget->surface.size = size;
    twtk_widget_dirty(widget);
}

void twtk_widget_dirty(twtk_widget_t *widget)
{
    assert(widget);
    // FIXME: atomic operations ?
    widget->flags |= TWTK_WIDGET_FLAG_DIRTY;

    if (widget->frame)
        twtk_widget_invalidate_rect(widget->frame, widget->viewport);
}

int twtk_widget_event(twtk_widget_t *widget, twtk_event_t *event, twtk_event_dispatch_t d)
{
    if (widget->cls->op_event == NULL)  // ignored / pass to childs
        return 0;

    int ret = widget->cls->op_event(widget, event, d);
    if (ret & TWTK_EVSTAT_DIRTY)
        twtk_widget_dirty(widget);

    return ret;
}

static int _invalidate_rect(twtk_widget_t *widget, twtk_rect_t rect, cairo_matrix_t *child_matrix)
{
    widget->flags |= TWTK_WIDGET_FLAG_DIRTY;

    if (widget->cls->op_invalidate_rect)
    {
        if (widget->cls->op_invalidate_rect(widget, rect, child_matrix))
            return 1;
    }

    if (widget->frame == NULL)
    {
        _DEBUG("invalidate_rect: (root)  widget=%10s rect=%4.0f:%4.0f:%4.0f:%4.0f/%4.0f",
            widget->name, rect.pos.x, rect.pos.y, rect.size.x, rect.size.y, rect.angle);

        return 0;
    }

    // FIXME: atomic operations ?
    // FIXME: clearly separate between content redrawn and recomposition

    cairo_matrix_t *widget_matrix = twtk_viewport_matrix(widget);

    cairo_matrix_t trans_matrix;
    cairo_matrix_multiply (&trans_matrix, child_matrix, widget_matrix);

#ifdef ENABLE_DEBUG
    twtk_dim_t new_x = rect.pos.x;
    twtk_dim_t new_y = rect.pos.y;
    cairo_matrix_transform_point(&trans_matrix, &new_x, &new_y);

    _DEBUG("invalidate_rect: (child) widget=%10s rect=%4.0f:%4.0f:%4.0f:%4.0f/%4.0f => %4.0f:%4.0f",
            widget->name, rect.pos.x, rect.pos.y, rect.size.x, rect.size.y, rect.angle, new_x, new_y);
#endif

    return _invalidate_rect(widget->frame, rect, &trans_matrix);
}

int twtk_widget_invalidate_rect(twtk_widget_t *widget, twtk_rect_t rect)
{
    if (widget == NULL)
        return 0;

    cairo_matrix_t matrix;
    cairo_matrix_init_identity(&matrix);

    _invalidate_rect(widget, rect, &matrix);

    return 0;
}

// FIXME: need to lock widget
int twtk_widget_set_frame(twtk_widget_t *widget, twtk_widget_t *target)
{
    if (widget == NULL)
        return 0;

    /** drop old framing, if existing **/
    if (widget->frame)
    {
        twtk_widget_list_remove_by_ref(&widget->frame->frames, widget);
        twtk_widget_invalidate_rect(widget->frame, widget->viewport);
        twtk_widget_unref(widget->frame);
        widget->frame = NULL;
    }

    /* default behaviour: add the child into the parent's frame */
    if (target)
    {
        twtk_widget_list_add(&target->frames, widget);
        widget->frame = twtk_widget_ref(target);
    }

    widget->flags &= ~(TWTK_WIDGET_FLAG_HAS_MATRIX | TWTK_WIDGET_FLAG_HAS_INV_MATRIX);

    return 0;
}

// FIXME: need to lock widget
int twtk_widget_set_parent(twtk_widget_t *widget, twtk_widget_t *target)
{
    if (widget == NULL)
        return 0;

    /** drop old framing, if existing **/
    if (widget->parent)
    {
        twtk_widget_list_remove_by_ref(&widget->childs, widget->parent);
        twtk_widget_unref(widget->parent);
        widget->parent = NULL;
    }

    /* default behaviour: add the child into the parent's frame */
    if (target)
    {
        twtk_widget_list_add(&target->childs, widget);
        widget->parent = twtk_widget_ref(target);
    }

    return 0;
}
