TRIPLE=mipsel-unknown-linux
LIBC=uclibc

TARGET=$(TRIPLE)-$(LIBC)
CROSS_BIN=$(HOME)/x-tools/$(TARGET)/bin

CC=$(CROSS_BIN)/$(TARGET)-gcc
AS=$(CROSS_BIN)/$(TARGET)-as
LD=$(CROSS_BIN)/$(TARGET)-gcc

OBJECTS += \
  cputorture-branch.s \
  cputorture-loadstore.s \
  cputorture-arith.s \
  cputorture-logic.s

LDFLAGS = \
  -nostdlib \
  -Ttext=0x1000 \
  -Wl,-e,_start


all: $(OBJECTS)
	$(CC) $(LDFLAGS) cputorture-branch.s -o cputorture-branch
	$(CC) $(LDFLAGS) cputorture-loadstore.s -o cputorture-loadstore
	$(CC) $(LDFLAGS) cputorture-arith.s -o cputorture-arith
	$(CC) $(LDFLAGS) cputorture-logic.s -o cputorture-logic

%.s: %.sm4
	m4 $< > $@

clean:
	rm -f *.s
	rm -f cputorture-branch
	rm -f cputorture-loadstore
	rm -f cputorture-arith
	rm -f cputorture-logic

# Ignore make implicit rules.
.SUFFIXES:
