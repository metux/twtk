#ifndef __TWTK_WIDGET_LIST_H_
#define __TWTK_WIDGET_LIST_H_

#include <pthread.h>
#include <twtk/types.h>

struct __twtk_widget_list_entry
{
    twtk_widget_t *widget;
    twtk_widget_list_entry_t *next;
    twtk_widget_list_entry_t *prev;
};

void twtk_widget_list_init(twtk_widget_list_t *list);
void twtk_widget_list_fini(twtk_widget_list_t *list);
void twtk_widget_list_add(twtk_widget_list_t *list, twtk_widget_t *widget, const char* name);
twtk_widget_t *twtk_widget_list_find_by_name(twtk_widget_list_t *list, const char* name);

int twtk_widget_list_find_pos(twtk_widget_list_t *list,
			      const twtk_vector_t pos,
			      twtk_widget_t **ret_widget,
			      twtk_vector_t *ret_pos,
			      char **ret_name);

#endif /* __TWTK_WIDGET_LIST_H_ */
