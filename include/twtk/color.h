#ifndef __TWTK_COLOR_H_
#define __TWTK_COLOR_H_

#include <inttypes.h>
#include <twtk/types.h>

struct __twtk_color
{
    unsigned char red;
    unsigned char green;
    unsigned char blue;
    unsigned char alpha;
};

#define TWTK_COLOR_RED		((twtk_color_t){.alpha = 0xff, .red = 0xff})
#define TWTK_COLOR_BLUE		((twtk_color_t){.alpha = 0xff, .blue = 0xff})
#define TWTK_COLOR_GREEN	((twtk_color_t){.alpha = 0xff, .green = 0xff})
#define TWTK_COLOR_WHITE	((twtk_color_t){.alpha = 0xff, .red = 0xff, .blue = 0xff, .green = 0xff})
#define TWTK_COLOR_BLACK	((twtk_color_t){.alpha = 0xff})
#define TWTK_COLOR_GREY		((twtk_color_t){.alpha = 0xff, .red = 0x70, .blue = 0x70, .green = 0x70})

#endif /* __TWTK_TYPES_H_ */
