
#define _GNU_SOURCE

#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <twtk/threads.h>

static int _initialized = 0;
static pthread_mutexattr_t _attr;

void _twtk_lock_init(pthread_mutex_t *mutex)
{
    if (!_initialized)
    {
	int r;

	r = pthread_mutexattr_init(&_attr);
	if (r)
	    fprintf(stderr, "failed to init mutexattr: %s\n", strerror(-r));

	r = pthread_mutexattr_settype(&_attr, PTHREAD_MUTEX_RECURSIVE);
	if (r)
	    fprintf(stderr, "failed to set attr type: %s\n", strerror(-r));

	_initialized = 1;
    }

    pthread_mutex_init(mutex, &_attr);
}
