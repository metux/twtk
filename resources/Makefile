
src_root = ../
src_dir  = src

include $(src_root)/config.mk
include $(src_root)/mk/pathes.mk

pixmaps = \
    penguin1.png \
    penguin2.png \
    button-abort.png \
    button-okay.png \
    button-cross-20x20.png \
    button-cross-80x80.png \
    button-1-red-80x80.png \
    button-2-red-80x80.png \
    button-3-red-80x80.png \
    button-4-red-80x80.png \
    button-1-green-80x80.png \
    button-2-green-80x80.png \
    button-3-green-80x80.png \
    button-4-green-80x80.png

all:

install: $(pixmaps)
	@mkdir -p $(DESTDIR)/$(CONFIG_PIXMAP_DIR)
	@cp $(pixmaps) $(DESTDIR)/$(CONFIG_PIXMAP_DIR)

clean:
