
CROSS_DIR=$(HOME)/x-tools
TARGET=mipsel-unknown-linux-uclibc
CROSS_BIN=$(CROSS_DIR)/$(TARGET)/bin

AR=$(CROSS_BIN)/$(TARGET)-ar
CC=$(CROSS_BIN)/$(TARGET)-gcc
AS=$(CROSS_BIN)/$(TARGET)-as
LD=$(CROSS_BIN)/$(TARGET)-gcc


