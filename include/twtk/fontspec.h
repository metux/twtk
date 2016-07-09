#ifndef __TWTK_FONTSPEC_H_
#define __TWTK_FONTSPEC_H_

#include <cairo.h>
#include <assert.h>
#include <twtk/types.h>
#include <twtk/color.h>


typedef struct __twtk_fontspec twtk_fontspec_t;

struct __twtk_fontspec
{
    char *name;
    float size;
    cairo_font_slant_t slant;
    cairo_font_weight_t weight;
    twtk_color_t color;
};

int twtk_fontspec_init_default(twtk_fontspec_t *fs)
    __attribute__((nonnull(1)));

int twtk_fontspec_fini(twtk_fontspec_t *fs)
    __attribute__((nonnull(1)));

int twtk_fontspec_set_name(twtk_fontspec_t *fs, const char *name)
    __attribute__((nonnull(1,2)));

int twtk_fontspec_set_size(twtk_fontspec_t *fs, float sz)
    __attribute__((nonnull(1)));

int twtk_fontspec_select_cr(const twtk_fontspec_t *fs, cairo_t *cr)
    __attribute__((nonnull(1,2)));

static int twtk_fonspec_set_color(twtk_fontspec_t *fs, twtk_color_t color)
    __attribute__((nonnull(1)));

static inline int twtk_fontspec_set_color(twtk_fontspec_t *fs, twtk_color_t color)
{
    assert(fs);
    fs->color = color;
    return 0;
}

static float twtk_fontspec_get_size(const twtk_fontspec_t *fs)
    __attribute__((nonnull(1)));

static inline float twtk_fontspec_get_size(const twtk_fontspec_t *fs)
{
    assert(fs);
    return fs->size;
}

#endif /* __TWTK_FONTSPEC_H_ */
