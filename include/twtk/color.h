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
                   { .alpha = 1,                       \
                     .red   = __TWTK_COLOR_CHANNEL(r), \
                     .green = __TWTK_COLOR_CHANNEL(g), \
                     .blue  = __TWTK_COLOR_CHANNEL(b) }

#define TWTK_COLORVAL_RED       __TWTK_COLOR_FROM_RGB(255,   0,   0)
#define TWTK_COLORVAL_BLUE      __TWTK_COLOR_FROM_RGB(0,     0, 255)
#define TWTK_COLORVAL_GREEN     __TWTK_COLOR_FROM_RGB(0,   255,   0)
#define TWTK_COLORVAL_WHITE     __TWTK_COLOR_FROM_RGB(255, 255, 255)
#define TWTK_COLORVAL_BLACK     __TWTK_COLOR_FROM_RGB(  0,   0,   0)
#define TWTK_COLORVAL_GREY      __TWTK_COLOR_FROM_RGB(127, 127, 127)
#define TWTK_COLORVAL_YELLOW    __TWTK_COLOR_FROM_RGB(255, 255,   0)
#define TWTK_COLORVAL_BROWN     __TWTK_COLOR_FROM_RGB(139,  19,  69)

#define TWTK_COLOR_RED          ((twtk_color_t)TWTK_COLORVAL_RED)
#define TWTK_COLOR_BLUE         ((twtk_color_t)TWTK_COLORVAL_BLUE)
#define TWTK_COLOR_GREEN        ((twtk_color_t)TWTK_COLORVAL_GREEN)
#define TWTK_COLOR_WHITE        ((twtk_color_t)TWTK_COLORVAL_WHITE)
#define TWTK_COLOR_BLACK        ((twtk_color_t)TWTK_COLORVAL_BLACK)
#define TWTK_COLOR_GREY         ((twtk_color_t)TWTK_COLORVAL_GREY)
#define TWTK_COLOR_YELLOW       ((twtk_color_t)TWTK_COLORVAL_YELLOW)
#define TWTK_COLOR_BROWN        ((twtk_color_t)TWTK_COLORVAL_BROWN)
#define TWTK_COLOR_TRANSPARENT  ((twtk_color_t){0})

#endif /* __TWTK_TYPES_H_ */
