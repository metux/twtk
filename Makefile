
MOUSE_DEVICE=/dev/input/by-id/usb-Logitech_USB_Optical_Mouse-event-mouse

PKG_CONFIG = PKG_CONFIG_PATH=/home/nekrad/.usr/cairo/lib/pkgconfig pkg-config 
CAIRO_LIBS = `$(PKG_CONFIG) --libs cairo cairo-png cairo-drm`
CAIRO_CFLAGS = `$(PKG_CONFIG) --cflags cairo cairo-png cairo-drm`

SRCS = \
    src/core/util.c			\
    src/core/threads.c			\
    src/core/widget.c			\
    src/core/widget-matrix.c		\
    src/core/widget-render.c		\
    src/core/widget-list.c		\
    src/platform/drm.c			\
    src/platform/generic.c		\
    src/platform/platform.c		\
    src/platform/debug-widget.c		\
    src/input/events.c			\
    src/input/evdev.c			\
    src/widgets/movebox-widget.c	\
    src/widgets/text-widget.c		\
    src/widgets/image-widget.c		\
    src/widgets/window-widget.c		\
    src/widgets/position-widget.c	\
    src/widgets/pattern-widget.c	\
    twtk_test.c				\

CFLAGS += -std=c99

# CFLAGS += -DENABLE_DEBUG_WIDGET
# CFLAGS += -DENABLE_DEBUG_WIDGET_LIST
# CFLAGS += -DENABLE_DEBUG_WIDGET_RENDER
# CFLAGS += -DENABLE_DEBUG_WIDGET_WINDOW
# CFLAGS += -DENABLE_DEBUG_WIDGET_MOVEBOX
# CFLAGS += -DENABLE_DEBUG_WIDGET_IMAGE
# CFLAGS += -DENABLE_DEBUG_WIDGET_TEXT
# CFLAGS += -DENABLE_DEBUG_PLATFORM_DRM

HEADERS	:= $(shell find ./include -name "*.h")

all:	twtk_test

dump:
	@echo $(CAIRO_LIBS)
	@echo $(CAIRO_CFLAGS)

clean:
	@rm -f twtk_test

twtk_test:	$(SRCS) $(HEADERS)
	@$(CC) $(SRCS) -o $@ $(CFLAGS) $(CAIRO_LIBS) $(CAIRO_CFLAGS) -Iinclude -levdev -pthread -lm

run:	twtk_test
	@LD_LIBRARY_PATH=/home/nekrad/.usr/cairo/lib TWTK_MOUSE_DEVICE=$(MOUSE_DEVICE) CAIRO_DRM_BASIC_FORCE=1 ./twtk_test

dbg:	twtk_test
	@LD_LIBRARY_PATH=/home/nekrad/.usr/cairo/lib TWTK_MOUSE_DEVICE=$(MOUSE_DEVICE) CAIRO_DRM_BASIC_FORCE=1 gdb ./twtk_test
