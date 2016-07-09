
#ifdef ENABLE_DEBUG_FONTSPEC
#define ENABLE_DEBUG
#endif

#define _GNU_SOURCE

#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <cairo.h>
#include <twtk/fontspec.h>


int twtk_fontspec_init_default(twtk_fontspec_t *fs)
{
    assert(fs);
    fs->name = strdup("monospace");
    fs->slant = CAIRO_FONT_SLANT_NORMAL;
    fs->weight = CAIRO_FONT_WEIGHT_NORMAL;
    fs->size = 20;
    fs->color = TWTK_COLOR_BLACK;
}

int twtk_fontspec_fini(twtk_fontspec_t *fs)
{
    assert(fs);

    if (fs->name)
        free(fs->name);

    *fs = (twtk_fontspec_t){ 0 };

    return 0;
}

int twtk_fontspec_set_name(twtk_fontspec_t *fs, const char *name)
{
    assert(fs);
    assert(name);

    if (fs->name)
        free(fs->name);

    fs->name = strdup(name);

    return 0;
}

int twtk_fontspec_set_size(twtk_fontspec_t *fs, float sz)
{
    assert(fs);
    fs->size = sz;

    return 0;
}

int twtk_fontspec_select_cr(const twtk_fontspec_t *fs, cairo_t *cr)
{
    assert(fs);
    assert(cr);

    cairo_select_font_face(cr, fs->name, fs->slant, fs->weight);
    cairo_set_font_size(cr, fs->size);

    cairo_set_source_rgba(
        cr,
        fs->color.red,
        fs->color.green,
        fs->color.blue,
        fs->color.alpha
    );

    return 0;
}
