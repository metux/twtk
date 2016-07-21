
#define _DEBUG_NAME	"widget-list"

#ifdef ENABLE_DEBUG_WIDGET_LIST
#define ENABLE_DEBUG
#endif

#define _GNU_SOURCE

#include <stdlib.h>
#include <assert.h>
#include <errno.h>
#include <twtk/threads.h>
#include <twtk/widget.h>
#include <twtk/widget-list.h>
#include <twtk/viewport.h>
#include <twtk-private/strutil.h>
#include <twtk-private/debug.h>


void twtk_widget_list_init(twtk_widget_list_t *list)
{
    assert(list);

    TWTK_INIT_LOCK(list);

    list->first = list->last = NULL;
    list->count = 0;
}

void twtk_widget_list_fini(twtk_widget_list_t *list)
{
    assert(list);

    // we dont need any locking here, as it may not be accessed from
    // somewhere else anymore
    twtk_widget_list_entry_t *walk = list->first;
    while (walk)
    {
	twtk_widget_list_entry_t *next = walk;
	twtk_widget_unref(walk->widget);
	free(walk);
	walk = next;
    }

    list->first = list->last = NULL;

    TWTK_DESTROY_LOCK(list)
}

void twtk_widget_list_add(twtk_widget_list_t *list, twtk_widget_t *widget)
{
    assert(list);
    assert(widget);

    // FIXME: should use free-list for more optimal allocation
    TWTK_NEW_OBJECT(twtk_widget_list_entry_t,ent);

    ent->widget = twtk_widget_ref(widget);

    TWTK_LOCK(list);

    if (list->first == NULL)
    {
	list->first = list->last = ent;
    }
    else
    {
	ent->prev = list->last;
	list->last->next = ent;
	list->last = ent;
    }

    TWTK_UNLOCK(list);
}

twtk_widget_t *twtk_widget_list_find_by_name(twtk_widget_list_t *list, const char* name)
{
    assert(list);
    assert(name);

    twtk_widget_list_entry_t *ent;

    TWTK_LOCK(list);

    for (ent=list->first; ent != NULL; ent=ent->next)
    {
	if ((ent->widget->name != NULL) && (strcmp(ent->widget->name, name)==0))
	{
	    twtk_widget_t *widget = twtk_widget_ref(ent->widget);
	    TWTK_UNLOCK(list);
	    return widget;
	}
    }

    TWTK_UNLOCK(list);

    return NULL;
}

int twtk_widget_list_find_pos(
    twtk_widget_list_t *list,
    const twtk_vector_t pos,
    twtk_widget_t **ret_widget,
    twtk_vector_t *ret_pos,
    char **ret_name)
{
    static char buffer[1024];

    assert(list);
    assert(ret_widget);
    assert(ret_pos);

    TWTK_LOCK(list);

    /* assume the last in list is the topmost */
    twtk_widget_list_entry_t *walk;
    for (walk = list->last; walk; walk = walk->prev)
    {
	assert(walk);
	assert(walk->widget);
	_DEBUG("find_pos: trying: %-15s ", walk->widget->name);
	if (twtk_viewport_pos_from_frame(walk->widget, pos, ret_pos))
	{
	    _DEBUG("find_pos: match:  %-15s pos=[%4.0f:%4.0f]",
		walk->widget->name,
		ret_pos->x,
		ret_pos->y
	    );
	    *ret_widget = twtk_widget_ref(walk->widget);
	    *ret_name = walk->widget->name;

	    TWTK_UNLOCK(list)
	    return 1;
	}
    }

    TWTK_UNLOCK(list);

    return 0;
}

static int __remove_by_ref(twtk_widget_list_t *list, twtk_widget_t *widget)
{
    twtk_widget_list_entry_t *old = NULL;

    /** special case: empty list **/
    if (list->first == NULL)
        return -ENOENT;

    /** special case: only one element **/
    if (list->first == list->last)
    {
        if (list->first->widget != widget)
            return -ENOENT;

        old = list->first;
        list->first = NULL;
        list->last = NULL;

        goto free_old;
    }

    /** special case: first element **/
    if (list->first->widget == widget)
    {
        old = list->first;
        list->first = old->next;
        list->first->prev = NULL;

        goto free_old;
    }

    /** special case: last element **/
    if (list->last->widget == widget)
    {
        old = list->last;
        old->prev->next = NULL;
        list->last = old->prev;

        goto free_old;
    }

    twtk_widget_list_entry_t *walk;
    for (walk = list->first; walk->next != NULL; walk = walk->next)
    {
        if (walk->next->widget == NULL)
        {
            old = walk->next;

            old->next->prev = walk;
            walk->next = old->next;

            if (walk->next == NULL)
                list->last = walk;

            goto free_old;
        }
    }

    return -ENOENT;

free_old:
    twtk_widget_unref(widget);
    free(old);
    return 0;
}

int twtk_widget_list_remove_by_ref(twtk_widget_list_t *list, twtk_widget_t *widget)
{
    assert(list);
    assert(widget);

    TWTK_LOCK(list);

    int ret = __remove_by_ref(list, widget);

    TWTK_UNLOCK(list);

    return ret;
}
