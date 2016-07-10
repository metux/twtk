#ifndef __TWTK_PRIVATE_DEBUG
#define __TWTK_PRIVATE_DEBUG

#include <stdio.h>

#ifdef ENABLE_DEBUG
#define _DEBUG(text, ...)       { fprintf(stderr, "[twtk/" _DEBUG_NAME "] " text "\n", ##__VA_ARGS__); }
#else
#define _DEBUG(text, ...)
#endif

#endif /* __TWTK_PRIVATE_DEBUG */
