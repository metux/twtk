#ifndef __TWTK_STRBUF_H
#define __TWTK_STRBUF_H

#include <malloc.h>
#include <string.h>
#include <memory.h>

typedef struct {
    char *value;    // pointer to the actual buffer
    char copy;      // we have an own copy if the string
} twtk_str_t;

static inline void twtk_str_free(twtk_str_t *sb)
{
    if (sb->value && sb->copy)
        free(sb->value);
    sb->value = NULL;
}

static inline void twtk_str_set_literal(twtk_str_t *sb, const char *str)
{
    if (sb->value && sb->copy)
        free(sb->value);

    *sb = (twtk_str_t){ .copy = 1, .value = (char*)str };
}

static inline void twtk_str_set_cpy(twtk_str_t *sb, const char* str)
{
    if (sb->value && sb->copy)
        free(sb->value);

    *sb = (twtk_str_t){ .copy = 0, .value = strdup(str) };
}

static inline int _twtk_do_strcmp(const char* s1, const char *s2)
{
    return strcmp(s1 ? s1 : "", s2 ? s2 : "");
}

static inline int twtk_str_cmpbuf(twtk_str_t *a, twtk_str_t *b) {
    return _twtk_do_strcmp(a->value, b->value);
}

static inline int twtk_str_cmplit(twtk_str_t *a, const char* b)
{
    return _twtk_do_strcmp(a->value, b);
}

static inline const char *twtk_str_get_direct(twtk_str_t *sb)
{
    return (sb->value ? sb->value : "");
}

static inline twtk_str_t twtk_str_literal(const char *str) {
    return (twtk_str_t) { .copy = 0, .value = (char*)str };
}

static inline twtk_str_t twtk_str_cpy(const char *str) {
    return (twtk_str_t) { .copy = 1, .value = strdup(str) };
}

#endif /* __TWTK_STRBUF_H */
