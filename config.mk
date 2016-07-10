CONFIG_PLATFORM_DRM := y
CONFIG_PLATFORM_XCB := y

CONFIG_MOUSE_DEVICE := /dev/input/by-id/usb-Logitech_USB_Optical_Mouse-event-mouse

PREFIX ?= /home/nekrad/.usr

PKG_CONFIG_PATH += /home/nekrad/.usr/cairo/lib/pkgconfig
