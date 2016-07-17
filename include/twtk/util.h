#ifndef __TWTK_UTIL_H
#define __TWTK_UTIL_H

#define TWTK_ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))

#define TWTK_OFFSETOF(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)

#define TWTK_CONTAINER_OF(ptr, type, member) ({            \
 const typeof( ((type *)0)->member ) *__mptr = (ptr);    \
 (type *)( (char *)__mptr - TWTK_OFFSETOF(type,member) );})

#endif /* __TWTK_UTIL_H */
