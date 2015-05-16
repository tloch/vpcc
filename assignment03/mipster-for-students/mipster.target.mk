# This file is generated by gyp; do not edit.

TOOLSET := target
TARGET := mipster
DEFS_Default :=

# Flags passed to all source files.
CFLAGS_Default := \
	-Wall \
	-Werror \
	-g \
	-O0 \
	-std=c++11 \
	-I/usr/include

# Flags passed to only C files.
CFLAGS_C_Default :=

# Flags passed to only C++ files.
CFLAGS_CC_Default :=

INCS_Default := \
	-I. \
	-Iemulator

OBJS := \
	$(obj).target/$(TARGET)/emulator/bin/main.o \
	$(obj).target/$(TARGET)/emulator/machine/memory.o \
	$(obj).target/$(TARGET)/emulator/machine/mips32.o \
	$(obj).target/$(TARGET)/emulator/machine/mips32_isa.o \
	$(obj).target/$(TARGET)/emulator/os/loader.o \
	$(obj).target/$(TARGET)/emulator/os/process.o \
	$(obj).target/$(TARGET)/emulator/os/os.o \
	$(obj).target/$(TARGET)/emulator/os/os_syscall.o

# Add to the list of files we specially track dependencies for.
all_deps += $(OBJS)

# CFLAGS et al overrides must be target-local.
# See "Target-specific Variable Values" in the GNU Make manual.
$(OBJS): TOOLSET := $(TOOLSET)
$(OBJS): GYP_CFLAGS := $(DEFS_$(BUILDTYPE)) $(INCS_$(BUILDTYPE))  $(CFLAGS_$(BUILDTYPE)) $(CFLAGS_C_$(BUILDTYPE))
$(OBJS): GYP_CXXFLAGS := $(DEFS_$(BUILDTYPE)) $(INCS_$(BUILDTYPE))  $(CFLAGS_$(BUILDTYPE)) $(CFLAGS_CC_$(BUILDTYPE))

# Suffix rules, putting all outputs into $(obj).

$(obj).$(TOOLSET)/$(TARGET)/%.o: $(srcdir)/%.cc FORCE_DO_CMD
	@$(call do_cmd,cxx,1)

# Try building from generated source, too.

$(obj).$(TOOLSET)/$(TARGET)/%.o: $(obj).$(TOOLSET)/%.cc FORCE_DO_CMD
	@$(call do_cmd,cxx,1)

$(obj).$(TOOLSET)/$(TARGET)/%.o: $(obj)/%.cc FORCE_DO_CMD
	@$(call do_cmd,cxx,1)

# End of this set of suffix rules
### Rules for final target.
LDFLAGS_Default :=

LIBS := \
	-lelf \
	-lgflags

$(builddir)/mipster: GYP_LDFLAGS := $(LDFLAGS_$(BUILDTYPE))
$(builddir)/mipster: LIBS := $(LIBS)
$(builddir)/mipster: LD_INPUTS := $(OBJS)
$(builddir)/mipster: TOOLSET := $(TOOLSET)
$(builddir)/mipster: $(OBJS) FORCE_DO_CMD
	$(call do_cmd,link)

all_deps += $(builddir)/mipster
# Add target alias
.PHONY: mipster
mipster: $(builddir)/mipster

# Add executable to "all" target.
.PHONY: all
all: $(builddir)/mipster

