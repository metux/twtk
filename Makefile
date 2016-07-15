
include ./config.mk


PREFIX          ?= /usr
LIBDIR          ?= $(PREFIX)/lib
PKGCONFIGDIR    ?= $(LIBDIR)/pkgconfig
PKG_CONFIG      ?= pkg-config
PKG_CONFIG_PATH ?= $(PKGCONFIGDIR)
PKG_CONFIG_CMD  := PKG_CONFIG_PATH=$(PKG_CONFIG_PATH) $(PKG_CONFIG)

## platform: DRM
pkgconfig-$(CONFIG_PLATFORM_DRM) +=	\
    cairo-drm

sources-$(CONFIG_PLATFORM_DRM) +=	\
    src/platform/drm.c

cflags-$(CONFIG_PLATFORM_DRM) +=	\
    -DENABLE_PLATFORM_DRM


## platform: XCB
pkgconfig-$(CONFIG_PLATFORM_XCB) +=	\
    xcb

sources-$(CONFIG_PLATFORM_XCB) +=	\
    src/platform/xcb.c			\
    src/platform/xcb-util.c

cflags-$(CONFIG_PLATFORM_XCB) +=	\
    -DENABLE_PLATFORM_XCB


## common

pkgconfig-y +=				\
    cairo

sources-y +=				\
    src/core/util.c			\
    src/core/threads.c			\
    src/core/fontspec.c			\
    src/core/widget.c			\
    src/core/widget-render.c		\
    src/core/widget-list.c		\
    src/core/menu.c			\
    src/core/viewport.c			\
    src/platform/generic.c		\
    src/platform/platform.c		\
    src/platform/debug-widget.c		\
    src/input/events.c			\
    src/input/evdev.c			\
    src/widgets/movebox-widget.c	\
    src/widgets/text-widget.c		\
    src/widgets/button-widget.c		\
    src/widgets/image-widget.c		\
    src/widgets/window-widget.c		\
    src/widgets/position-widget.c	\
    src/widgets/pattern-widget.c	\
    twtk_test.c				\

cflags-y +=				\
    -Iinclude				\
    -std=c99				\
    -pthread

libs-y +=				\
    -levdev				\
    -pthread				\
    -lm

LIBS   += $(libs-y)   `$(PKG_CONFIG_CMD) --libs   $(pkgconfig-y)`
CFLAGS += $(cflags-y) `$(PKG_CONFIG_CMD) --cflags $(pkgconfig-y)`

# CFLAGS += -DENABLE_WIDGET_CLIPPING
# CFLAGS += -DENABLE_DEBUG_FONTSPEC
# CFLAGS += -DENABLE_DEBUG_WIDGET
# CFLAGS += -DENABLE_DEBUG_WIDGET_LIST
# CFLAGS += -DENABLE_DEBUG_WIDGET_RENDER
# CFLAGS += -DENABLE_DEBUG_WIDGET_WINDOW
# CFLAGS += -DENABLE_DEBUG_WIDGET_MOVEBOX
# CFLAGS += -DENABLE_DEBUG_WIDGET_IMAGE
# CFLAGS += -DENABLE_DEBUG_WIDGET_BUTTON
# CFLAGS += -DENABLE_DEBUG_WIDGET_TEXT
# CFLAGS += -DENABLE_DEBUG_PLATFORM_GENERIC
# CFLAGS += -DENABLE_DEBUG_PLATFORM_DRM
# CFLAGS += -DENABLE_DEBUG_PLATFORM_XCB
CFLAGS += -DENABLE_CLIP_DIRTY
CFLAGS += -DNDEBUG


HEADERS	:= $(shell find ./include -name "*.h")


all:	twtk_test

dump:
	@echo "PKG_CONFIG_PATH=$(PKG_CONFIG_PATH)"
	@echo "pkgconfig-y=$(pkgconfig-y)"
	@echo "CFLAGS=$(CFLAGS)"
	echo "LIBS=$(LIBS)"

clean:
	@rm -f twtk_test

twtk_test:	$(sources-y) $(HEADERS)
	@$(CC) $(sources-y) -o $@ $(CFLAGS) $(LIBS)

run:	twtk_test
	@LD_LIBRARY_PATH=/home/nekrad/.usr/cairo/lib TWTK_MOUSE_DEVICE="$(CONFIG_MOUSE_DEVICE)" CAIRO_DRM_BASIC_FORCE=1 ./twtk_test

dbg:	twtk_test
	@LD_LIBRARY_PATH=/home/nekrad/.usr/cairo/lib TWTK_MOUSE_DEVICE="$(CONFIG_MOUSE_DEVICE)" CAIRO_DRM_BASIC_FORCE=1 gdb ./twtk_test
