CONFIG_PLATFORM_DRM := y
CONFIG_PLATFORM_XCB := y

CONFIG_CAIROEXTRA_JPEG := y

CONFIG_PKG_JPEG_CFLAGS ?=
CONFIG_PKG_JPEG_LIBS   ?= -ljpeg

CONFIG_MOUSE_DEVICE := /dev/input/by-id/usb-Logitech_USB_Optical_Mouse-event-mouse

PREFIX ?= /home/nekrad/.usr

PKG_CONFIG_PATH += /home/nekrad/.usr/cairo/lib/pkgconfig
