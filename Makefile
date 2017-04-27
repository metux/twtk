
include ./config.mk

SUBDIRS = src resources twtk_test screen_test pngview

all:
	@for d in $(SUBDIRS) ; do $(MAKE) -C $$d all ; done

clean:
	@for d in $(SUBDIRS) ; do $(MAKE) -C $$d clean ; done

install:
	@for d in $(SUBDIRS) ; do $(MAKE) -C $$d install; done
