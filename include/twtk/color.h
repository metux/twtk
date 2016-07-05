#ifndef __TWTK_COLOR_H_
#define __TWTK_COLOR_H_

#include <inttypes.h>

struct __twtk_color
{
    float red;
    float green;
    float blue;
    float alpha;
};

#define __TWTK_COLOR_CHANNEL(x) ((float)x/256)

#define __TWTK_COLOR_FROM_RGB(r,g,b)                   \
    ((twtk_color_t){ .alpha = 1,                       \
                     .red   = __TWTK_COLOR_CHANNEL(r), \
                     .green = __TWTK_COLOR_CHANNEL(g), \
                     .blue  = __TWTK_COLOR_CHANNEL(b) })

#define TWTK_COLOR_RED          __TWTK_COLOR_FROM_RGB(255,   0,   0)
#define TWTK_COLOR_BLUE         __TWTK_COLOR_FROM_RGB(0,     0, 255)
#define TWTK_COLOR_GREEN        __TWTK_COLOR_FROM_RGB(0,   255,   0)
#define TWTK_COLOR_WHITE        __TWTK_COLOR_FROM_RGB(255, 255, 255)
#define TWTK_COLOR_BLACK        __TWTK_COLOR_FROM_RGB(  0,   0,   0)
#define TWTK_COLOR_GREY         __TWTK_COLOR_FROM_RGB(127, 127, 127)
#define TWTK_COLOR_YELLOW       __TWTK_COLOR_FROM_RGB(255, 255,   0)
#define TWTK_COLOR_BROWN        __TWTK_COLOR_FROM_RGB(139,  19,  69)

#endif /* __TWTK_TYPES_H_ */
