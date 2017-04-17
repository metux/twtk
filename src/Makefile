
src_root = ../
src_dir  = src

include $(src_root)/config.mk
include $(src_root)/mk/pathes.mk

library_name = twtk

## platform: DRM
pkgconfig-$(CONFIG_PLATFORM_DRM) +=	cairo-drm libevdev
sources-$(CONFIG_PLATFORM_DRM) +=	platform/drm.c
cflags-priv-$(CONFIG_PLATFORM_DRM) +=	-DENABLE_PLATFORM_DRM

## platform: XCB
pkgconfig-$(CONFIG_PLATFORM_XCB) +=	xcb
cflags-priv-$(CONFIG_PLATFORM_XCB) +=	-DENABLE_PLATFORM_XCB
sources-$(CONFIG_PLATFORM_XCB) +=	platform/xcb.c \
					platform/xcb-util.c

## image loader: jpeg
libs-$(CONFIG_IMAGE_JPEG) +=		$(CONFIG_PKG_JPEG_LIBS)
cflags-$(CONFIG_IMAGE_JPEG) +=		$(CONFIG_PKG_JPEG_CFLAGS)
cflags-priv-$(CONFIG_IMAGE_JPEG) +=	-DENABLE_IMAGE_JPEG

## cairo extras that havent been mainlined yet
sources-$(CONFIG_CAIROEXTRA_JPEG) +=	cairoextra/cairo-jpeg.c
includes-$(CONFIG_CAIROEXTRA_JPEG) +=	$(src_root)/$(src_dir)/cairoextra
cflags-$(CONFIG_CAIROEXTRA_JPEG) +=	$(CONFIG_PKG_JPEG_CFLAGS)
libs-$(CONFIG_CAIROEXTRA_JPEG) +=	$(CONFIG_PKG_JPEG_LIBS)

## common

pkgconfig-y += cairo

sources-y += \
    core/util.c \
    core/threads.c \
    core/fontspec.c \
    core/widget.c \
    core/widget-render.c \
    core/widget-list.c \
    core/menu.c \
    core/viewport.c \
    platform/generic.c \
    platform/platform.c \
    platform/debug-widget.c \
    input/events.c \
    input/evdev.c \
    widgets/movebox-widget.c \
    widgets/text-widget.c \
    widgets/button-widget.c \
    widgets/image-widget.c \
    widgets/window-widget.c \
    widgets/position-widget.c \
    widgets/pattern-widget.c

includes-y += $(src_root)/include

cflags-y += -pthread

libs-y += -pthread

include $(src_root)/mk/library.mk
