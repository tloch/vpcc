.PHONY: test default
default: test


# BEGIN CROSS COMMON
CROSS_DIR=$(HOME)/x-tools
TARGET=mipsel-unknown-linux-uclibc
CROSS_BIN=$(CROSS_DIR)/$(TARGET)/bin

AR=$(CROSS_BIN)/$(TARGET)-ar
CC=$(CROSS_BIN)/$(TARGET)-gcc
AS=$(CROSS_BIN)/$(TARGET)-as
LD=$(CROSS_BIN)/$(TARGET)-gcc
# END CROSS COMMON




test:
	chmod +x do_scanner_demo.sh
	./do_scanner_demo.sh

#	cd mipster
#	make
#	cd userland/skeleton-scanner/
#	make
#	../../out/Default/mipster a.out <scanner.c

