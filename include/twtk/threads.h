#ifndef __TWTK_THREADS_H
#define __TWTK_THREADS_H

#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <twtk/types.h>

void _twtk_lock_init(pthread_mutex_t *mutex);

#define TWTK_INIT_LOCK(obj)	_twtk_lock_init(&obj->lock);
#define TWTK_LOCK(obj)								\
    if (1) {									\
	int ret = pthread_mutex_trylock(&obj->lock);				\
	if (ret)								\
	    fprintf(stderr, "TWTK_LOCK: failed to lock: %s\n", strerror(-ret));	\
    }

#define TWTK_UNLOCK(obj)	pthread_mutex_unlock(&obj->lock);
#define TWTK_DESTROY_LOCK(obj)	pthread_mutex_destroy(&obj->lock);

#endif /* __TWTK_THREADS_H */
