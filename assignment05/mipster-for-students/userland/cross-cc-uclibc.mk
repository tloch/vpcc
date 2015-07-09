CFLAGS += \
  -mips32r2 \
  -EL \
  -static \
  -mno-shared \
  -O0 \
  -msoft-float


LDFLAGS += \
  -mips32r2 \
  -EL \
  -static \
  -mno-shared

LDADD += -lc

all: $(OBJECTS)
	$(LD) $(LDFLAGS) -o a.out $(OBJECTS) $(LDADD)

%.o : %.c
	$(CC) $< $(CFLAGS) -c -o $@

clean:
	rm -f *.o a.out *.i *.s

# Ignore make implicit rules.
.SUFFIXES:

