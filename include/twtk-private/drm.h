#ifndef __TWTK_PRIVATE_DRM_H
#define __TWTK_PRIVATE_DRM_H

#include <stdint.h>
#include <twtk/platform.h>

typedef struct
{
    twtk_platform_t base;

    twtk_widget_t *root;

    int fd;
    uint32_t crtc_id;
    uint32_t cursor_bo;
    void *cursor_mapped;
} twtk_platform_drm_t;

int twtk_platform_drm_initcursor(twtk_platform_drm_t *platform);

twtk_platform_t *twtk_platform_drm_init();

#endif /* __TWTK_PRIVATE_DRM_H */
