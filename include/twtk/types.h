#ifndef __TWTK_TYPES_H_
#define __TWTK_TYPES_H_

#include <pthread.h>
#include <stdlib.h>

typedef struct __twtk_widget		twtk_widget_t;
typedef struct __twtk_widget_class	twtk_widget_class_t;
typedef struct __twtk_vector		twtk_vector_t;
typedef struct __twtk_rect		twtk_rect_t;
typedef struct __twtk_event		twtk_event_t;
typedef struct __twtk_platform		twtk_platform_t;
typedef struct __twtk_color		twtk_color_t;
typedef struct __twtk_widget_list	twtk_widget_list_t;
typedef struct __twtk_widget_list_entry	twtk_widget_list_entry_t;
typedef struct __twtk_menu		twtk_menu_t;
typedef struct __twtk_menu_entry	twtk_menu_entry_t;

typedef pthread_mutex_t			twtk_lock_t;

/** dimension values **/
typedef double				twtk_dim_t;

static inline twtk_dim_t twtk_dim_max(twtk_dim_t a, twtk_dim_t b)
{
    return ((a > b) ? a : b);
}

#define TWTK_ALLOC_OBJECT(type)		\
    ((type*)calloc(sizeof(type),1))

#define TWTK_ALLOC_ARRAY(type,cnt)	\
    ((type*)calloc(sizeof(type),cnt))

#define TWTK_NEW_OBJECT(type,name)	\
    type *name = ((type*)calloc(sizeof(type),1))

#endif /* __TWTK_TYPES_H_ */
