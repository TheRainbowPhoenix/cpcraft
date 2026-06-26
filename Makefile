SOURCEDIR = src
BUILDDIR = obj
OUTDIR = dist
DEPDIR = .deps

AS:=sh4a_nofpueb-elf-gcc
AS_FLAGS:=-gdwarf-5

SDK_DIR?=/sdk

DEPFLAGS=-MT $@ -MMD -MP -MF $(DEPDIR)/$*.d
WARNINGS=-Wall -Wextra -pedantic -Werror -pedantic-errors
INCLUDES=-I$(SDK_DIR)/include -Isrc
DEFINES=
# -Ofast matches QBos07's benchmark. -fno-builtin keeps our memcpy/memset
# loops from being replaced with library calls (we want them inlined so
# the LTO pass can see and pipeline them).
FUNCTION_FLAGS=-flto=auto -ffat-lto-objects -fno-builtin -ffunction-sections -fdata-sections -gdwarf-5 -Ofast
COMMON_FLAGS=$(FUNCTION_FLAGS) $(INCLUDES) $(WARNINGS) $(DEFINES)

CC:=sh4a_nofpueb-elf-gcc
# -std=c23 keeps us future-proof; -fno-strict-aliasing is needed because
# the engine reinterprets uint16_t* / uintptr_t in the blit hot path.
CC_FLAGS=-std=c23 -fno-strict-aliasing $(COMMON_FLAGS)

CXX:=sh4a_nofpueb-elf-g++
CXX_FLAGS=-std=c++20 $(COMMON_FLAGS)

LD:=sh4a_nofpueb-elf-g++
LD_FLAGS:=$(FUNCTION_FLAGS) -Wl,--gc-sections
LIBS:=-L$(SDK_DIR) -lsdk

READELF:=sh4a_nofpueb-elf-readelf
OBJCOPY:=sh4a_nofpueb-elf-objcopy
STRIP:=sh4a_nofpueb-elf-strip

APP_ELF := $(OUTDIR)/CPCraft.elf
APP_HH3 := $(APP_ELF:.elf=.hh3)

AS_SOURCES:=$(shell find $(SOURCEDIR) -name '*.S')
CC_SOURCES:=$(shell find $(SOURCEDIR) -name '*.c')
CXX_SOURCES:=$(shell find $(SOURCEDIR) -name '*.cpp')
OBJECTS := $(addprefix $(BUILDDIR)/,$(AS_SOURCES:.S=.o)) \
	$(addprefix $(BUILDDIR)/,$(CC_SOURCES:.c=.o)) \
	$(addprefix $(BUILDDIR)/,$(CXX_SOURCES:.cpp=.o))

NOLTOOBJS := $(foreach obj, $(OBJECTS), $(if $(findstring /nolto/, $(obj)), $(obj)))

DEPFILES := $(OBJECTS:$(BUILDDIR)/%.o=$(DEPDIR)/%.d)

hh3: $(APP_HH3) Makefile
elf: $(APP_ELF) Makefile

all: elf hh3
.DEFAULT_GOAL := all
.SECONDARY: # Prevents intermediate files from being deleted

.NOTPARALLEL: clean
clean:
	rm -rf $(BUILDDIR) $(OUTDIR) $(DEPDIR)

%.hh3: %.elf
	$(STRIP) -o $@ $^

$(APP_ELF): $(OBJECTS)
	@mkdir -p $(dir $@)
	$(LD) -Wl,-Map $@.map -o $@ $(LD_FLAGS) $^ $(LIBS)

$(NOLTOOBJS): FUNCTION_FLAGS+=-fno-lto

$(BUILDDIR)/%.o: %.S
	@mkdir -p $(dir $@)
	$(AS) -c $< -o $@ $(AS_FLAGS)

$(BUILDDIR)/%.o: %.c
	@mkdir -p $(dir $@)
	@mkdir -p $(dir $(DEPDIR)/$<)
	+$(CC) -c $< -o $@ $(CC_FLAGS) $(DEPFLAGS)

$(BUILDDIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	@mkdir -p $(dir $(DEPDIR)/$<)
	+$(CXX) -c $< -o $@ $(CXX_FLAGS) $(DEPFLAGS)

compile_commands.json:
	$(MAKE) $(MAKEFLAGS) clean
	bear -- sh -c "$(MAKE) $(MAKEFLAGS) --keep-going all || exit 0"

# Simulator target. Delegates to simulator/Makefile, which builds a native
# (SDL2) or web (Emscripten) binary that runs the engine against stub SDK
# headers. See simulator/README.md for details.
#
# Examples:
#   make sim            - build the native simulator
#   make sim-run        - build and run the native simulator
#   make sim-web        - build the web simulator (requires emcc on PATH)
#   make sim-clean      - clean simulator build artifacts
sim:
	$(MAKE) -C simulator

sim-run:
	$(MAKE) -C simulator run

sim-web:
	$(MAKE) -C simulator web

sim-clean:
	$(MAKE) -C simulator clean

# Host-side test target. Compiles the engine against stub SDK headers and
# runs the test harness. Requires gcc on the host.
#
# By default uses the stubs bundled under test/sdk-stub/. Override
# NATIVE_SDK_STUB to point at the real SDK headers if you want to test
# against the actual declarations (still requires a runtime stub of the
# function-pointer symbols, which the test harness provides).
NATIVE_SDK_STUB ?= test/sdk-stub
test:
	gcc -std=c23 -O2 -fno-strict-aliasing -I src -I $(NATIVE_SDK_STUB) \
	    -Wall -Wextra -Wno-pedantic -Wno-unused-function -Wno-comment \
	    -D'__attribute__(x)=' -D'__asm__(x)=' \
	    -include stdint.h -include stdbool.h -include stddef.h \
	    src/engine/*.c src/demo/*.c test/native_test.c \
	    -o /tmp/cpcraft-port-test -lm
	/tmp/cpcraft-port-test

.PHONY: elf hh3 all clean compile_commands.json test sim sim-run sim-web sim-clean

-include $(DEPFILES)