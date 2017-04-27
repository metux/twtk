
src_root = ..
src_dir = touchtest

include $(src_root)/config.mk
include $(src_root)/mk/pathes.mk
-include $(src_root)/src/libtwtk.auto.mk

prog_name = twtk-touchtest

sources-y += twtk-touchtest.c

include $(src_root)/mk/prog.mk

install:
	@mkdir -p $(DESTDIR)$(BINDIR)
	@cp $(prog_name) $(DESTDIR)$(BINDIR)
	@chmod +x $(DESTDIR)$(BINDIR)/$(prog_name)
