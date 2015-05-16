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

LDADD +=
OBJECTS += 
BINARY?=a.out


%.o : %.c
	$(CC) $< $(CFLAGS) -c -o $@

cross_clean:
	rm -f *.o
	rm -f $(BINARY)

cross_link: $(OBJECTS)
	$(LD) $(LDFLAGS) -o $(BINARY) $(OBJECTS) $(LDADD)

# Ignore make implicit rules.
.SUFFIXES:

