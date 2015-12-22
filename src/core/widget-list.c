
#define _DEBUG_NAME	"widget-list"

#include <stdlib.h>
#include <assert.h>
#include <twtk/threads.h>
#include <twtk/widget.h>
#include <twtk/widget-list.h>
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
	STRBUF_FREE(walk->name);
	twtk_widget_unref(walk->widget);
	free(walk);
	walk = next;
    }

    list->first = list->last = NULL;

    TWTK_DESTROY_LOCK(list)
}

void twtk_widget_list_add(twtk_widget_list_t *list, twtk_widget_t *widget, const char* name)
{
    assert(list);
    assert(widget);

    // FIXME: should use free-list for more optimal allocation
    twtk_widget_list_entry_t *ent = (twtk_widget_list_entry_t*)calloc(1,sizeof(twtk_widget_list_entry_t));
    STRBUF_SET(ent->name, name);
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
	if ((ent->name != NULL) && (strcmp(ent->name, name)))
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
	_DEBUG("trying: %-15s ", walk->name);
	if (_twtk_widget_translate_pos(walk->widget, pos, ret_pos))
	{
	    _DEBUG("match:  %-15s pos=[%4.0f:%4.0f]",
		walk->name,
		ret_pos->x,
		ret_pos->y
	    );
	    *ret_widget = twtk_widget_ref(walk->widget);
	    *ret_name = walk->name;

	    TWTK_UNLOCK(list)
	    return 1;
	}
    }

    TWTK_UNLOCK(list);

    return 0;
}
