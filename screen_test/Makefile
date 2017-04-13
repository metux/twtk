
src_root = ..
src_dir = screen_test

include $(src_root)/config.mk
include $(src_root)/mk/pathes.mk
-include $(src_root)/src/libtwtk.auto.mk

prog_name = screen_test

libs-y += -lrt

sources-y += screen_test.c

include $(src_root)/mk/prog.mk

install:
	@mkdir -p $(DESTDIR)$(BINDIR)
	@cp screen_test $(DESTDIR)$(BINDIR)
	@chmod +x $(DESTDIR)$(BINDIR)/screen_test
