#ifndef __TWTK_TYPES_H_
#define __TWTK_TYPES_H_

#include <pthread.h>

typedef struct __twtk_widget		twtk_widget_t;
typedef struct __twtk_widget_class	twtk_widget_class_t;
typedef struct __twtk_vector		twtk_vector_t;
typedef struct __twtk_rect		twtk_rect_t;
typedef struct __twtk_event		twtk_event_t;
typedef struct __twtk_platform		twtk_platform_t;
typedef struct __twtk_color		twtk_color_t;
typedef struct __twtk_widget_list	twtk_widget_list_t;
typedef struct __twtk_widget_list_entry	twtk_widget_list_entry_t;

typedef pthread_mutex_t			twtk_lock_t;

#endif /* __TWTK_TYPES_H_ */
