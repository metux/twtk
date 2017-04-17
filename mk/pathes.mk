PREFIX          ?= /usr
LIBDIR          ?= $(PREFIX)/lib
DATADIR         ?= $(PREFIX)/share
BINDIR          ?= $(PREFIX)/bin
INCLUDEDIR      ?= $(PREFIX)/include
PKGCONFIGDIR    ?= $(LIBDIR)/pkgconfig
PKG_CONFIG      ?= pkg-config
PKG_CONFIG_PATH ?= $(PKGCONFIGDIR)
AR              ?= ar

_ppath := $(shell echo "$(PKG_CONFIG_PATH)" | sed -e 's~ ~:~g')
PKG_CONFIG_CMD  := PKG_CONFIG_PATH=$(_ppath) $(PKG_CONFIG)
