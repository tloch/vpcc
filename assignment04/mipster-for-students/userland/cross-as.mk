CFLAGS =\
  -mips32r2 \
  -EL

LDFLAGS = \
  -nostdlib \
  -static \
  -mips32r2 \
  -mno-shared \
  -EL

LDADD=\
  -lc

all: $(OBJECTS)
	$(LD) $(LDFLAGS) -o a.out $(OBJECTS) $(LDADD)

%.o : %.c
	$(CC) $< $(CFLAGS) -c -o $@

%.o: %.s
	$(AS) $< $(CFLAGS) -o $@

clean:
	rm -f *.o a.out *.i

# Ignore make implicit rules.
.SUFFIXES:

