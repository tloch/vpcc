include ../cross-common.mk

OBJECTS += crt0.o syscalls.o

CFLAGS =\
  -mips32r2 \
  -EL

LDFLAGS = \
  -nostdlib \
  -static \
  -mips32r2 \
  -mno-shared \
  -EL

all: $(OBJECTS)
	$(AR) cr mipster.a $(OBJECTS)

crt0.o: crt0.S
	$(CC) crt0.S $(CFLAGS) -c -o crt0.o

%.o: %.c
	$(CC) $< $(CFLAGS) -c -o $@

clean:
	rm -f *.o
	rm -f *.a

# Ignore make implicit rules.
.SUFFIXES:
