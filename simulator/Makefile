# simulator/Makefile
#
# Build the cpcraft-port simulator for native (Linux/macOS/Windows) or web
# (Emscripten). The simulator runs the engine against stub SDK headers
# that implement the LCD protocol in software and render to an SDL2 window.
#
# Targets:
#   make            - build the native simulator (./cpcraft-port-sim)
#   make run        - build and run the native simulator
#   make web        - build the web simulator (./web/cpcraft-port.html)
#   make clean      - remove build artifacts
#
# Environment variables:
#   SDL2_DIR    - path to a local SDL2 installation (optional; if set,
#                 we use -I$(SDL2_DIR)/include -L$(SDL2_DIR)/lib)
#   EMCC        - path to emcc (default: emcc on PATH)
#
# This Makefile is self-contained — it doesn't depend on the parent
# Makefile. The parent Makefile has a `make sim` target that delegates
# to this one.

# --- Paths ---
ROOT_DIR     := ..
SRC_DIR      := $(ROOT_DIR)/src
ENGINE_DIR   := $(SRC_DIR)/engine
DEMO_DIR     := $(SRC_DIR)/demo
SIM_SRC_DIR  := src
SIM_INCLUDE  := include
OBJ_DIR      := obj
OUT_DIR      := out
WEB_DIR      := web

# --- Compiler / linker ---
CXX := g++
CC  := gcc
LD  := g++

# SDL2 detection. If SDL2_DIR is set, use it; otherwise rely on the
# system SDL2 (or sdl2-config). MUST come before CXXFLAGS/CFLAGS below
# so those can incorporate SDL2_CFLAGS.
ifdef SDL2_DIR
SDL2_CFLAGS := -I$(SDL2_DIR)/include -D_REENTRANT
SDL2_LIBS   := -L$(SDL2_DIR)/lib -lSDL2
else
SDL2_CFLAGS := $(shell sdl2-config --cflags 2>/dev/null) -D_REENTRANT
SDL2_LIBS   := $(shell sdl2-config --libs 2>/dev/null)
# If sdl2-config wasn't found, fall back to -lSDL2 and hope it's on the
# default search path.
ifeq ($(SDL2_CFLAGS), -D_REENTRANT)
SDL2_LIBS := -lSDL2
endif
endif

# Common flags. Note -I$(SIM_INCLUDE) comes BEFORE -I$(SRC_DIR) so the
# simulator's <sdk/...> stubs are picked up instead of any real SDK.
CXXFLAGS := -std=c++20 -O2 -Wall -Wextra -Wno-unused-parameter \
	    -I$(SIM_INCLUDE) -I$(SRC_DIR) $(SDL2_CFLAGS)
CFLAGS   := -std=c11 -O2 -Wall -Wextra -Wno-unused-parameter \
	    -I$(SIM_INCLUDE) -I$(SRC_DIR) $(SDL2_CFLAGS)

# --- Sources ---
ENGINE_SRCS := $(wildcard $(ENGINE_DIR)/*.c)
DEMO_SRCS   := $(wildcard $(DEMO_DIR)/*.c)
APP_MAIN    := $(SRC_DIR)/main.cpp
SIM_SRCS    := $(SIM_SRC_DIR)/simulator.cpp $(SIM_SRC_DIR)/main_simulator.cpp

# --- Objects ---
# main.cpp is compiled with -Dmain=app_main so its main() becomes app_main()
# and our main_simulator.cpp can provide the real main().
OBJ_DIR_NATIVE := $(OBJ_DIR)/native
ENGINE_OBJS := $(patsubst $(ENGINE_DIR)/%.c,$(OBJ_DIR_NATIVE)/engine/%.o,$(ENGINE_SRCS))
DEMO_OBJS   := $(patsubst $(DEMO_DIR)/%.c,$(OBJ_DIR_NATIVE)/demo/%.o,$(DEMO_SRCS))
APP_MAIN_OBJ := $(OBJ_DIR_NATIVE)/main_app.o
SIM_OBJS    := $(patsubst $(SIM_SRC_DIR)/%.cpp,$(OBJ_DIR_NATIVE)/sim/%.o,$(SIM_SRCS))

NATIVE_OBJS := $(ENGINE_OBJS) $(DEMO_OBJS) $(APP_MAIN_OBJ) $(SIM_OBJS)
NATIVE_OUT  := $(OUT_DIR)/cpcraft-port-sim

# --- Native build ---
.PHONY: all run clean web clean-web
all: $(NATIVE_OUT)

$(NATIVE_OUT): $(NATIVE_OBJS)
	@mkdir -p $(dir $@)
	$(LD) -o $@ $^ $(SDL2_LIBS) -lm

$(OBJ_DIR_NATIVE)/engine/%.o: $(ENGINE_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR_NATIVE)/demo/%.o: $(DEMO_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# The app's main() — renamed to app_main() so main_simulator.cpp can
# provide the real main().
$(OBJ_DIR_NATIVE)/main_app.o: $(SRC_DIR)/main.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -Dmain=app_main -c $< -o $@

$(OBJ_DIR_NATIVE)/sim/%.o: $(SIM_SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

run: $(NATIVE_OUT)
	./$(NATIVE_OUT)

# --- Web build (Emscripten) ---
EMCC ?= emcc
WEB_OUT := $(WEB_DIR)/cpcraft-port.html

web: $(WEB_OUT)

$(WEB_OUT): $(ENGINE_SRCS) $(DEMO_SRCS) $(SRC_DIR)/main.cpp $(SIM_SRCS) $(WEB_DIR)/shell.html
	@mkdir -p $(WEB_DIR)
	$(EMCC) -std=c++20 -O2 -Wall -Wextra -Wno-unused-parameter \
	    -I$(SIM_INCLUDE) -I$(SRC_DIR) \
	    -Dmain=app_main -D__EMSCRIPTEN__ \
	    $(ENGINE_SRCS) $(DEMO_SRCS) $(SRC_DIR)/main.cpp $(SIM_SRCS) \
	    -sUSE_SDL=2 \
	    --shell-file $(WEB_DIR)/shell.html \
	    -sALLOW_MEMORY_GROWTH=1 \
	    -sASYNCIFY=1 \
	    -o $(WEB_OUT)

clean-web:
	rm -f $(WEB_OUT) $(WEB_DIR)/cpcraft-port.js $(WEB_DIR)/cpcraft-port.wasm

clean: clean-web
	rm -rf $(OBJ_DIR) $(OUT_DIR)
