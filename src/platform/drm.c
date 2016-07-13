
#define _DEBUG_NAME	"drm"

#ifdef ENABLE_DEBUG_PLATFORM_DRM
#define ENABLE_DEBUG
#endif

#include <unistd.h>
#include <stdint.h>
#include <inttypes.h>
#include <stdio.h>
#include <sys/mman.h>
#include <string.h>
#include <errno.h>
#include <libdrm/drm.h>
#include <xf86drmMode.h>
#include <xf86drm.h>
#include <malloc.h>
#include <cairo-drm.h>
#include <assert.h>
#include <semaphore.h>

#include <twtk/events.h>
#include <twtk/widget.h>
#include <twtk/color.h>
#include <twtk-private/drm.h>
#include <twtk-private/debug.h>
#include <twtk-private/platform-generic.h>

#define I	0xffffffff
#define O	0x0

#define CURSOR_DIM	64

static const uint32_t drm_cursor[CURSOR_DIM*CURSOR_DIM] = {
/*                8               16              24              32              40              48              56              64 */
    I,I,I,I,I,I,I,I,I,I,I,I,I,I,I,I,0,0,0,0,0,0,0,0,0,0,0,0,0,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,
    I,I,I,I,I,I,I,I,I,I,I,I,I,I,I,0,0,0,0,0,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,
    I,I,I,I,I,I,I,I,I,I,I,I,I,I,0,0,0,0,0,0,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,
    I,I,I,I,I,O,O,O,O,O,O,O,0,0,0,0,0,0,0,0,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,
    I,I,I,I,I,I,O,O,O,O,O,O,0,0,0,0,0,0,0,0,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,
    I,I,I,O,I,I,I,O,O,O,O,O,0,0,0,0,0,0,0,0,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,
    I,I,I,O,O,I,I,I,O,O,O,O,0,0,0,0,0,0,0,0,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,
    I,I,I,O,O,O,I,I,I,O,O,O,0,0,0,0,0,0,0,0,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,
    I,I,I,O,O,O,O,I,I,I,O,O,0,0,0,0,0,0,0,0,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,
    I,I,I,O,O,O,O,O,I,I,I,O,0,0,0,0,0,0,0,0,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,
    I,I,I,O,O,O,O,O,O,I,I,I,0,0,0,0,0,0,0,0,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,
    I,I,I,O,O,O,O,O,O,O,I,I,I,0,0,0,0,0,0,0,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,
    I,I,I,0,0,0,0,0,0,0,0,I,I,I,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,
    I,I,I,0,0,0,0,0,0,0,0,0,I,I,O,O,O,O,O,0,0,0,0,0,0,0,0,0,0,0,0,0,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,
    I,I,0,0,0,0,0,0,0,0,0,0,0,O,I,O,O,O,O,0,0,0,0,0,0,0,0,0,0,0,0,0,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,
    I,0,0,0,0,0,0,0,0,0,0,0,0,O,O,I,O,O,O,0,0,0,0,0,0,0,0,0,0,0,0,0,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,
    0,0,0,0,0,0,0,0,0,0,0,0,0,O,O,O,O,O,O,0,0,0,0,0,0,0,0,0,0,0,0,0,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,
    0,0,0,0,0,0,0,0,0,0,0,0,0,O,O,O,O,O,O,0,0,0,0,0,0,0,0,0,0,0,0,0,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,
    0,0,0,0,0,0,0,0,0,0,0,0,0,O,O,O,O,O,O,0,0,0,0,0,0,0,0,0,0,0,0,0,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,
    O,O,O,O,O,O,O,O,O,O,O,O,0,0,0,0,0,0,0,0,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,
    O,O,O,O,O,O,O,O,O,O,O,O,0,0,0,0,0,0,0,0,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,
    O,O,O,O,O,O,O,O,O,O,O,O,0,0,0,0,0,0,0,0,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,
    O,O,O,O,O,O,O,O,O,O,O,O,0,0,0,0,0,0,0,0,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,
    O,O,O,O,O,O,O,O,O,O,O,O,0,0,0,0,0,0,0,0,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,
    O,O,O,O,O,O,O,O,O,O,O,O,0,0,0,0,0,0,0,0,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,
    O,O,O,O,O,O,O,O,O,O,O,O,0,0,0,0,0,0,0,0,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,
    O,O,O,O,O,O,O,O,O,O,O,O,0,0,0,0,0,0,0,0,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,
    O,O,O,O,O,O,O,O,O,O,O,O,0,0,0,0,0,0,0,0,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,
    O,O,O,O,O,O,O,O,O,O,O,O,0,0,0,0,0,0,0,0,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,
    O,O,O,O,O,O,O,O,O,O,O,O,0,0,0,0,0,0,0,0,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,
    O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,
    O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,
    O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,
    O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,
    O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,
    O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,
    O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,
    O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,
    O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,
    O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,
    O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,
    O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,
    O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,
    O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,
    O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,
    O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,
    O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,
    O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,
    O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,
    O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,
    O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,
    O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,
    O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,
    O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,
    O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,
    O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,
    O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,
    O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,
    O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,
    O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,
    O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,
    O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,
};

int twtk_drm_cursor_handler(twtk_event_t* ev, void *p)
{
    twtk_platform_drm_t *inf = (twtk_platform_drm_t *)p;

    if ((ev->type == TWTK_EVENT_MOUSE) &&
	(ev->mouse.event == TWTK_EVENT_MOUSE_MOVE))
    {
	int ret = drmModeMoveCursor(inf->fd, inf->crtc_id, ev->mouse.pos.x, ev->mouse.pos.y);
	if (ret)
	    _DEBUG("movecursor failed: %s", strerror(-ret));
    }
}

int twtk_drm_initcursor(twtk_platform_drm_t *inf)
{
    int ret;

    struct drm_mode_create_dumb creq = {
	.width = CURSOR_DIM,
	.height = CURSOR_DIM,
	.bpp = 32
    };

    if ((ret = drmIoctl(inf->fd, DRM_IOCTL_MODE_CREATE_DUMB, &creq) < 0))
    {
	_DEBUG("initcursor: %s", strerror(-ret));
	return -1;
    }

    inf->cursor_bo = creq.handle;

    struct drm_mode_map_dumb mreq = {
	.handle = creq.handle
    };

    if (ret = drmIoctl(inf->fd, DRM_IOCTL_MODE_MAP_DUMB, &mreq))
    {
	_DEBUG("initcursor map dump: %s", strerror(-ret));
	return -1;
    }

    _DEBUG("cursor: dumb bo id: %d", mreq.handle);
    _DEBUG("cursor: crtc id: %d", inf->crtc_id);

    void *ptr = mmap(0, creq.size, PROT_READ|PROT_WRITE, MAP_SHARED, inf->fd, mreq.offset);
    if (ptr == MAP_FAILED)
    {
	_DEBUG("initcursor mmap() failed: %s", strerror(errno));
    }

    inf->cursor_mapped = ptr;

    memcpy(ptr, drm_cursor, creq.size);
    _DEBUG("copied bitmap");

    ret = drmModeSetCursor(inf->fd, inf->crtc_id, mreq.handle, CURSOR_DIM, CURSOR_DIM);
    if (ret)
    {
	_DEBUG("setcursor failed: %s", strerror(-ret));
	return 0;
    }

    ret = drmModeMoveCursor(inf->fd, inf->crtc_id, -100, -100);
    if (ret)
    {
	_DEBUG("movecursor failed: %s", strerror(-ret));
	return 0;
    }

    twtk_event_notify("drm-cursor", twtk_drm_cursor_handler, inf);

    _DEBUG("cursor done ...");

    return 1;
}

static int _drm_destroy(twtk_platform_t *platform)
{
    assert(platform);
    cairo_surface_destroy(platform->surface);

    twtk_platform_drm_t *drm_platform = (twtk_platform_drm_t*) platform;
    sem_destroy(&drm_platform->mainloop_sem);

    // FIXME: should free the cursor
    // FIXME: free the device

    free(platform);
    return 0;
}

static int _drm_mainloop(twtk_platform_t *platform)
{
    assert(platform);
    twtk_platform_drm_t *drm_platform = (twtk_platform_drm_t*) platform;
    sem_wait(&drm_platform->mainloop_sem);

    return 0;
}

static int _drm_stop(twtk_platform_t *platform)
{
    assert(platform);
    twtk_platform_drm_t *drm_platform = (twtk_platform_drm_t*) platform;
    sem_post(&drm_platform->mainloop_sem);

    return 0;
}

twtk_platform_t *twtk_platform_drm_init()
{
    TWTK_NEW_OBJECT(twtk_platform_drm_t, platform);

    if (!(platform->base.device = cairo_drm_device_default())) {
        _DEBUG("failed to open DRM device");
        return NULL;
    }

    if (!(platform->base.surface = cairo_drm_surface_create(platform->base.device, CAIRO_FORMAT_ARGB32, 1366, 768))) {
        _DEBUG("failed to allocate surface");
        return NULL;
    }

    cairo_device_t *dev = cairo_surface_get_device(platform->base.surface);

    sem_init(&platform->mainloop_sem, 0, 0);

    platform->fd = cairo_drm_device_get_fd(dev);
    platform->crtc_id = cairo_drm_surface_get_crtc_id(platform->base.surface);

    platform->base.op_destroy      = _drm_destroy;
    platform->base.op_get_root     = _twtk_platform_generic_get_root;
    platform->base.op_get_context  = _twtk_platform_generic_get_context;
    platform->base.op_free_context = _twtk_platform_generic_free_context;
    platform->base.op_mainloop     = _drm_mainloop;
    platform->base.op_stop         = _drm_stop;
    platform->base.op_map_widget   = _twtk_platform_generic_map_widget;

    twtk_drm_initcursor(platform);

    return (twtk_platform_t*)platform;
}
