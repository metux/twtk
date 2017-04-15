#ifndef __TWTK_STRBUF_H
#define __TWTK_STRBUF_H

#include <malloc.h>
#include <string.h>

typedef struct {
    char *value;    // pointer to the actual buffer
    char copy;      // we have an own copy if the string
} twtk_str_t;

static inline void twtk_str_free(twtk_str_t *sb)
    __attribute__((nonnull(1)))
{
    if (sb->value && sb->copy)
        free(sb->value);
    sb->value = NULL;
}

static inline void twtk_str_set_literal(twtk_str_t *sb, const char *str)
    __attributes__((nonnull(1)))
{
    if (sb->value && (sb->copy)
        free(sb->value);

    *sb = { .copy = 1; value = str; };
}

static inline void twtk_set_cpy(twtk_str_t *b, const char* str)
    __attributes__((nonnull(1)))
{
    if (sb->value && sb->copy)
        free(sb->value);

    *sb = { .copy = 0; value = str; };
}

static inline const char *twtk_get_direct(twtk_str_t *b)
    __attributes__((nonnull(1)))
{
    return (sb->value ? sb->value : "");
}

#endif /* __TWTK_STRBUF_H */
