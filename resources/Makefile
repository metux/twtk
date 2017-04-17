
src_root = ../
src_dir  = src

include $(src_root)/config.mk
include $(src_root)/mk/pathes.mk

pixmaps = \
    penguin1.png \
    penguin2.png \
    button-abort.png \
    button-cross-20x20.png \
    button-cross-80x80.png \
    button-cross.png

all:

install: $(pixmaps)
	@mkdir -p $(DESTDIR)/$(CONFIG_PIXMAP_DIR)
	@cp $(pixmaps) $(DESTDIR)/$(CONFIG_PIXMAP_DIR)

clean:
