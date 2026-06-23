# run `make all` to compile the .hhk and .bin file, use `make` to compile only the .bin file.
# The .hhk file is the original format, the bin file is a newer format.
APP_NAME:=CPDoom

SDK_DIR?=/sdk

AS:=sh4a_nofpueb-elf-gcc
AS_FLAGS:=

COMMON_FLAGS:=-Ofast -gdwarf-5 -ffunction-sections -fdata-sections -flto=auto -ffat-lto-objects -fno-strict-aliasing
INCLUDES:=-I $(SDK_DIR)/include/
WARNINGS:=-Wall -Wextra -Werror -Wno-missing-field-initializers -Wno-alloc-size-larger-than

CC:=sh4a_nofpueb-elf-gcc
CC_FLAGS:=$(COMMON_FLAGS) $(INCLUDES) $(WARNINGS) -std=gnu18

CXX:=sh4a_nofpueb-elf-g++
CXX_FLAGS:=-fno-exceptions -fno-rtti $(COMMON_FLAGS) $(INCLUDES) $(WARNINGS) -std=gnu++20

LD:=$(CXX)
LD_FLAGS:=$(COMMON_FLAGS) $(WARNINGS) -Wl,-Ttext-segment,0x8C052800 -Wl,--section-start,.end_mem=8cfefffc -Wno-undef -Wl,--gc-sections -fno-lto #-v

READELF:=sh4a_nofpueb-elf-readelf
OBJCOPY:=sh4a_nofpueb-elf-objcopy
STRIP:=sh4a_nofpueb-elf-strip

SOURCEDIR = src
BUILDDIR = obj
OUTDIR = dist

AS_SOURCES:=$(shell find $(SOURCEDIR) -name '*.S')
CC_SOURCES:=$(shell find $(SOURCEDIR) -name '*.c')
CXX_SOURCES:=$(shell find $(SOURCEDIR) -name '*.cpp')
OBJECTS := $(addprefix $(BUILDDIR)/,$(AS_SOURCES:.S=.o)) \
	$(addprefix $(BUILDDIR)/,$(CC_SOURCES:.c=.o)) \
	$(addprefix $(BUILDDIR)/,$(CXX_SOURCES:.cpp=.o))

APP_HH3:=$(OUTDIR)/$(APP_NAME).hh3
APP_ELF:=$(OUTDIR)/$(APP_NAME).elf

.DEFAULT_GOAL=all

hh3: $(APP_HH3) Makefile
elf: $(APP_ELF) Makefile

all: $(APP_ELF) $(APP_HH3) Makefile

clean:
	rm -rf $(BUILDDIR) $(OUTDIR)

%.hh3: %.elf
	$(STRIP) -o $@ $^

$(APP_ELF): $(OBJECTS) $(SDK_DIR)/libsdk.a
	mkdir -p $(dir $@)
	$(LD) -Wl,-Map $@.map -o $@ $(LD_FLAGS) $(OBJECTS) -L$(SDK_DIR) -lsdk

# We're not actually building sdk.o, just telling the user they need to do it
# themselves. Just using the target to trigger an error when the file is
# required but does not exist.
$(SDK_DIR)/libsdk.a:
	@echo "You need to build the SDK before using it. Run make in the SDK directory, and check the README.md in the SDK directory for more information" && exit 1

$(BUILDDIR)/%.o: %.S
	mkdir -p $(dir $@)
	$(AS) -c $< -o $@ $(AS_FLAGS)

$(BUILDDIR)/%.o: %.c
	mkdir -p $(dir $@)
	$(CC) -c $< -o $@ $(CC_FLAGS)

# Break the build if global constructors are present:
# Read the sections from the object file (with readelf -S) and look for any
# called .ctors - if they exist, give the user an error message, delete the
# object file (so that on subsequent runs of make the build will still fail)
# and exit with an error code to halt the build.
$(BUILDDIR)/%.o: %.cpp
	mkdir -p $(dir $@)
	$(CXX) -c $< -o $@ $(CXX_FLAGS)

.PHONY: elf hh3 all clean
