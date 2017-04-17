CONFIG_PLATFORM_DRM    ?= y
CONFIG_PLATFORM_XCB    ?= y
CONFIG_CAIROEXTRA_JPEG ?= n
CONFIG_IMAGE_JPEG      ?= n

CONFIG_PKG_JPEG_CFLAGS ?=
CONFIG_PKG_JPEG_LIBS   ?= -ljpeg

CONFIG_PIXMAP_DIR      ?= $(DATADIR)/pixmaps/twtk

CONFIG_MOUSE_DEVICE    ?= /dev/input/by-id/usb-1bcf_USB_Optical_Mouse-event-mouse
PKG_CONFIG_PATH        += /home/nekrad/.usr/cairo/lib/pkgconfig

## additional debug flags - for hacking only
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
# CFLAGS += -DENABLE_DEBUG_WIDGET_MENU
# CFLAGS += -DENABLE_DEBUG_PLATFORM_GENERIC
# CFLAGS += -DENABLE_DEBUG_PLATFORM_DRM
# CFLAGS += -DENABLE_DEBUG_PLATFORM_XCB
CFLAGS += -DENABLE_CLIP_DIRTY
CFLAGS += -DNDEBUG
