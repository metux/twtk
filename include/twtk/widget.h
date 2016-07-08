#ifndef __TWTK_WIDGET_H_
#define __TWTK_WIDGET_H_

#include <inttypes.h>
#include <cairo.h>
#include <twtk/types.h>
#include <twtk/threads.h>
#include <twtk/types.h>
#include <twtk/vector.h>
#include <twtk/rect.h>
#include <twtk/color.h>
#include <twtk/widget-list.h>

#define TWTK_WIDGET_FLAG_DIRTY	1 << 0
#define TWTK_WIDGET_FLAG_POPUP	1 << 1

/**
 * widget event handler return mask
 */

/* event has been handled */
#define TWTK_EVSTAT_HANDLED	1 << 0

/* event had been consumed - dont pass to childs */
#define TWTK_EVSTAT_CONSUMED	1 << 1

/* event had been *changed* - reanalyze before passing to childs */
#define TWTK_EVSTAT_MODIFIED	1 << 2

/* event caused widget to be changed, need repaint */
#define TWTK_EVSTAT_DIRTY	1 << 3

/**
 * widget event dispatching points
 */

/* event will be sent before subframes are checked */
typedef enum
{
    /* event is dispatched before subframes are tried */
    TWTK_EVENT_DISPATCH_BEFORE = 1 << 0,

    /* event is dispatched after subframes are tried */
    TWTK_EVENT_DISPATCH_AFTER  = 1 << 2,

    /* event is dispatched directly to the frame */
    TWTK_EVENT_DISPATCH_DIRECT = 1 << 3,
} twtk_event_dispatch_t;

/**
 * widget operation callbacks
 *
 * these callbacks are always with the widget lock held - they may **NOT** lock them any further
 * (we're currently not using recursive locks yet)
 */
typedef int (*twtk_widget_op_event_t)(twtk_widget_t *widget, twtk_event_t *event, twtk_event_dispatch_t d)
    __attribute__((nonnull(1,2)));
typedef int (*twtk_widget_op_paint_t)(twtk_widget_t *widget, cairo_t *ctx);
typedef int (*twtk_widget_op_fini_t)(twtk_widget_t *widget);
typedef int (*twtk_widget_op_set_str_t)(twtk_widget_t *widget, const char* name, const char* value);
typedef int (*twtk_widget_op_set_uint_t)(twtk_widget_t *widget, const char* name, uint32_t value);
typedef int (*twtk_widget_op_set_float_t)(twtk_widget_t *widget, const char* name, double value);
typedef int (*twtk_widget_op_set_color_t)(twtk_widget_t *widget, const char* name, twtk_color_t value);
typedef int (*twtk_widget_op_insert_child_t)(twtk_widget_t *widget, twtk_widget_t* child);

struct __twtk_widget_class
{
    /**
     * generic event receiver (eg. mouse)
     * note that the events are already translated into the widget's realm (eg. coordinates)
     */
    twtk_widget_op_event_t op_event;

    /**
     * paint the widget onto (virtual) surface
     * its called by the platform engine at will. do *NOT* call it explicitly
     */
    twtk_widget_op_paint_t op_paint;

    /**
     * destructor for the widget - will be called right before actual destruction
     * (when refcnt reached zero). it shall *only* free private resources.
     * the priv structure is free'd by the engine
     */
    twtk_widget_op_fini_t op_fini;

    /**
     * set an string-type class-specific attribte
     */
    twtk_widget_op_set_str_t op_set_str;

    /**
     * set an uint32-type class-specific attribute
     */
    twtk_widget_op_set_uint_t op_set_uint;

    /**
     * set an color-type class-specific attribute
     */
    twtk_widget_op_set_color_t op_set_color;

    /**
     * set an float-type class-specific attribute
     */
    twtk_widget_op_set_float_t op_set_float;

    /**
     * insert an (newly added) child into a frame
     * it the vector is NULL, the engine will use the widget itself as frame
     * the function is only called if the child is a popup
     */
    twtk_widget_op_insert_child_t op_insert_child;

    uint64_t magic;
    size_t priv_size;
    const char* name;
};

struct __twtk_widget_list
{
    int count;
    twtk_widget_list_entry_t *first;
    twtk_widget_list_entry_t *last;
    pthread_mutex_t lock;
};

struct __twtk_widget
{
    uint32_t flags;

    twtk_lock_t lock;

    /* viewport within the parent frame */
    twtk_rect_t viewport;

    /* virtual surface */
    twtk_rect_t surface;

    twtk_color_t background_color;

    const twtk_widget_class_t *cls;
    void *priv;
    int refcnt;
    char *name;

    twtk_widget_t *parent;
    twtk_widget_t *frame;

    twtk_widget_list_t childs;
    twtk_widget_list_t frames;

    /* render cache */
    cairo_pattern_t *paint_cache;
};

twtk_widget_t *twtk_widget_alloc(const twtk_widget_class_t *cls);

int twtk_widget_do_draw(twtk_widget_t *widget, cairo_t *cr);

#define TWTK_WIDGET_PRIV(widget, type)	\
    ((type)(widget->priv))

void twtk_widget_destroy (twtk_widget_t *widget);

static inline twtk_widget_t *twtk_widget_ref(twtk_widget_t *widget)
{
    TWTK_LOCK(widget);
    widget->refcnt++;		// FIXME: use atomic operations ?
    TWTK_UNLOCK(widget);

    return widget;
}

static inline void twtk_widget_unref(twtk_widget_t *widget)
{
    TWTK_LOCK(widget);
    widget->refcnt--;		// FIXME: use atomic operations ?
    TWTK_UNLOCK(widget)

    // no locking needed here anymore - once it reaches zero, nobody
    // should have a ref to it
    if (widget->refcnt < 1)
    {
        printf("destroying widget: %s\n", widget->name);
	twtk_widget_destroy(widget);
    }
    {
//        printf("widget refcnt: %d -> %s\n", widget->refcnt, widget->name);
    }
}

/**
 * Mark the widget as dirty - aka: needs repaint
 */
void twtk_widget_dirty(twtk_widget_t *widget);

int  twtk_widget_set_name(twtk_widget_t *widget, const char *name);
int  twtk_widget_set_border_color(twtk_widget_t *widget, twtk_color_t color);
int  twtk_widget_set_background_color(twtk_widget_t *widget, twtk_color_t color);
int  twtk_widget_set_font_color(twtk_widget_t *widget, twtk_color_t color);
int  twtk_widget_set_font_size(twtk_widget_t *widget, twtk_dim_t size);
int  twtk_widget_set_border_width(twtk_widget_t *widget, twtk_dim_t size);
int  twtk_widget_set_font_name(twtk_widget_t *widget, const char* n);
int  twtk_widget_set_text(twtk_widget_t *widget, const char* text);


/*
 * set the widget's viewport within parent frame
 */
void twtk_widget_set_viewport(twtk_widget_t *widget, twtk_rect_t vp)
    __attribute__((nonnull(1)));

static inline void twtk_widget_set_viewport_coords(
    twtk_widget_t *widget,
    twtk_dim_t x,
    twtk_dim_t y,
    twtk_dim_t w,
    twtk_dim_t h,
    twtk_dim_t a
)
{
    twtk_widget_set_viewport(
        widget,
        twtk_rect_by_coords(x, y, w, h, a)
    );
}

/*
 * set an string attribute
 */
int  twtk_widget_set_str(twtk_widget_t *widget, const char *name, const char *value)
    __attribute__((nonnull(1,2)));

void twtk_widget_move(twtk_widget_t *widget, twtk_vector_t pos)
    __attribute__((nonnull(1)));

static inline void twtk_widget_move_coords(twtk_widget_t *widget, twtk_dim_t x, twtk_dim_t y)
{
    twtk_widget_move(widget, TWTK_VECTOR(x, y));
}

void twtk_widget_move_rel(twtk_widget_t *widget, twtk_vector_t pos)
    __attribute__((nonnull(1)));

static inline void twtk_widget_move_rel_coords(twtk_widget_t *widget, twtk_dim_t x, twtk_dim_t y)
{
    twtk_widget_move_rel(widget, TWTK_VECTOR(x, y));
}

void twtk_widget_resize(twtk_widget_t *widget, twtk_vector_t size)
    __attribute__((nonnull(1)));

static inline void twtk_widget_resize_coords(twtk_widget_t *widget, twtk_dim_t x, twtk_dim_t y)
{
    twtk_widget_resize(widget, TWTK_VECTOR(x, y));
}

void twtk_widget_rotate(twtk_widget_t *widget, twtk_dim_t a);

void twtk_widget_vresize(twtk_widget_t *widget, twtk_vector_t size)
    __attribute__((nonnull(1)));

static inline void twtk_widget_vresize_coords(twtk_widget_t *widget, twtk_dim_t x, twtk_dim_t y)
{
    twtk_widget_vresize(widget, TWTK_VECTOR(x, y));
}

int  twtk_widget_event(twtk_widget_t *widget, twtk_event_t *event, twtk_event_dispatch_t d)
    __attribute__((nonnull(1,2)));

int twtk_widget_dispatch(twtk_widget_t *parent, twtk_event_t *event)
    __attribute__((nonnull(1,2)));

/**
 * low level functions for managing parent and frame references
 *
 * parameter target may be NULL
 */
int  twtk_widget_set_parent(twtk_widget_t *widget, twtk_widget_t *target)
    __attribute__((nonnull(1)));
int  twtk_widget_set_frame(twtk_widget_t *widget, twtk_widget_t *target)
    __attribute__((nonnull(1)));

/**
 * Invalidate an rect within the given widget
 * (will be repainted in next iteration)
 *
 * @param widget if null, do nothing
 */
int  twtk_widget_invalidate_rect(twtk_widget_t *widget, twtk_rect_t rect);

/**
 * Add widget as a child
 * The child widget's refcount will be increased.
 */
void twtk_widget_add_child(twtk_widget_t *parent, twtk_widget_t *child, const char* name);

/**
 * Add child and then de-reference
 *
 * Useful for chaining like this:
 *   twtk_widget_add_child_unref(parent, twtk_text_widget_create(...));
 */
void twtk_widget_add_child_unref(twtk_widget_t *parent, twtk_widget_t *child, const char *name);

#endif /* __TWTK_WIDGET_H_ */
