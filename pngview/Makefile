
src_root = ..
src_dir = pngview

include $(src_root)/config.mk
include $(src_root)/mk/pathes.mk
-include $(src_root)/src/libtwtk.auto.mk

prog_name = twtk-pngview

sources-y += twtk-pngview.c

include $(src_root)/mk/prog.mk

install:
	@mkdir -p $(DESTDIR)$(BINDIR)
	@cp $(prog_name) $(DESTDIR)$(BINDIR)
	@chmod +x $(DESTDIR)$(BINDIR)/$(prog_name)
