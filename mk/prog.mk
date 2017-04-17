
TARGET_OBJS := $(sources-y:.c=.o)

LIBS   += $(lib_twtk_LIBS) $(libs-y)
CFLAGS += $(cflags-y) $(lib_twtk_CFLAGS)

CFLAGS += $(addprefix -I,$(includes-y)) \
          -DPIXMAP_DIR=\"$(CONFIG_PIXMAP_DIR)\"

all:	$(prog_name)

dump:
	@echo "PKG_CONFIG_PATH=$(PKG_CONFIG_PATH)"
	@echo "CFLAGS=$(CFLAGS)"
	@echo "LIBS=$(LIBS)"
	@echo "pkgconfig-y=$(pkgconfig-y)"

clean:
	@rm -f $(prog_name)
	@find -name "*.o" -delete
	@find -name "*.a" -delete

$(prog_name):	$(TARGET_OBJS)
	$(CC) -o $@ $(CFLAGS) $(TARGET_OBJS) $(LIBS)

run:	$(prog_name)
	@TWTK_PIXMAP_DIR=$(src_root)/resources TWTK_MOUSE_DEVICE="$(CONFIG_MOUSE_DEVICE)" CAIRO_DRM_BASIC_FORCE=1 ./$(prog_name)

dbg:	$(prog_name)
	@TWTK_PIXMAP_DIR=$(src_root)/resources TWTK_MOUSE_DEVICE="$(CONFIG_MOUSE_DEVICE)" CAIRO_DRM_BASIC_FORCE=1 gdb ./$(prog_name)
