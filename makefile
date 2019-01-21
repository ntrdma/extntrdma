ifndef KSRC
KSRC		= /lib/modules/$(shell uname -r)/build
endif
V		= 0

# Build the ntrdma kernel module
LINUXCONFIG	+= DEBUG=1
LINUXCONFIG	+= CONFIG_NTC=m
LINUXCONFIG	+= CONFIG_NTC_NTB_MSI=m
LINUXCONFIG	+= CONFIG_NTC_NTB_DMA_REQ_IMM=1
LINUXCONFIG	+= CONFIG_NTC_TCP=m
LINUXCONFIG	+= CONFIG_NTC_PHYS=m
LINUXCONFIG	+= CONFIG_NTRDMA=m
LINUXCONFIG	+= CONFIG_NTRDMA_RETRY_RECV=1

LINUXINCLUDE	= \
	-I$$(srctree)/arch/$$(SRCARCH)/include \
	-Iarch/$$(SRCARCH)/include/generated/uapi \
	-Iarch/$$(SRCARCH)/include/generated \
	$$(if $$(KBUILD_SRC), -I$$(srctree)/include) \
	-I$(CURDIR)/include -Iinclude \
	-I$$(srctree)/arch/$$(SRCARCH)/include/uapi \
	-Iarch/$$(SRCARCH)/include/generated/uapi \
	-I$$(srctree)/include/uapi \
	-Iinclude/generated/uapi \
	-include $$(srctree)/include/linux/kconfig.h \
	-include $(CURDIR)/config.h

MAKE_TARGETS	= all modules modules_install clean help
MAKE_OPTIONS	= -C $(KSRC) M=$(CURDIR) V=$(V) $(LINUXCONFIG)

modules: config.h
modules: MAKE_OPTIONS += LINUXINCLUDE='$(LINUXINCLUDE)'

install: modules_install

config.h: makefile
	@echo > config.h "/* Generated by Makefile */"
	@for E in ${LINUXCONFIG}; do echo >> config.h "#define $${E/=/ }" ; done

build_test: synopsis_ntrdma_ring test_ntrdma_ring

test: build_test
	./synopsis_ntrdma_ring
	./test_ntrdma_ring

$(MAKE_TARGETS):
	$(MAKE) $(MAKE_OPTIONS) $@

.PHONY: $(MAKE_TARGETS)

