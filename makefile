# Add new targets here (<name>_TARGET) and add the new target variable to the PROJECTS variable.
REGEXP_TARGET = regexp
UUID_TARGET   = uuid
UDP_TARGET    = udp

# Define projects to build (some might not build on all platforms). Add **all** targtes to
# PROJECTS and exclude them later (see below) where SUPPORTED_OS is set.
PROJECTS = $(REGEXP_TARGET) $(UUID_TARGET) $(UDP_TARGET)

# Currently used sqlite version (merely informational purpose).
SQLITE_VERSION = 3.41.2

# Each target has its own source dir.
REGEXP_SRC = $(wildcard regexp/*.c)
UUID_SRC   = $(wildcard uuid/*.c)
UDP_SRC    = $(wildcard udp/*.c)

OUT_DIR    = bin

# detect the current dev environment
https://stackoverflow.com/a/52062069
https://stackoverflow.com/a/14777895
# os agnostic mkdir
https://stackoverflow.com/a/30225575
ifeq '$(findstring ;,$(PATH))' ';'
    UNAME := Windows
	mkdir = @mkdir $(subst /,\,$(1)) >nul 2>&1 || (exit 0)
	rmdir = @rmdir /s /q $(subst /,\,$(1)) >nul 2>&1 || (exit 0)
else
    UNAME := $(shell uname 2>/dev/null || echo Unknown)
    UNAME := $(patsubst CYGWIN%,Cygwin,$(UNAME))
    UNAME := $(patsubst MSYS%,MSYS,$(UNAME))
    UNAME := $(patsubst MINGW%,MSYS,$(UNAME))
	mkdir = @mkdir -p $(1)
	rmdir = @rm -rf $(1)
endif

# Default targeting operating system (os).
ifeq ($(UNAME),Windows)
SUPPORTED_OS = win32 win64
else ifeq ($(UNAME),Cygwin)
SUPPORTED_OS = cygwin win32 win64
else
SUPPORTED_OS = unix
# Can't build 'udp' on unix yet.
PROJECTS := $(filter-out $(UDP_TARGET),$(PROJECTS))
endif
SUPPORTED_TARGETS = $(SUPPORTED_OS) clean ls-os

# Error if targeting os is not in supported os list
ifeq (,$(findstring $(MAKECMDGOALS),$(SUPPORTED_TARGETS)))
$(error Target OS '$(MAKECMDGOALS)' is not supported use `make ls-os` to get a list of supported targets)
endif

CFLAGS         = -Isqlite -Wall
LFLAGS         = -s -shared

# parse command line arguments
ifdef DEBUG
	CFLAGS += -DDEBUG
	OUT_DIR    := $(OUT_DIR)/$(MAKECMDGOALS)/debug
else
	OUT_DIR    := $(OUT_DIR)/$(MAKECMDGOALS)/release
endif

# Prepend directory to target files
REGEXP_TARGET := $(OUT_DIR)/$(REGEXP_TARGET)
UUID_TARGET   := $(OUT_DIR)/$(UUID_TARGET)
UDP_TARGET    := $(OUT_DIR)/$(UDP_TARGET)

all: $(PROJECTS)
	@echo "Built for os '$(MAKECMDGOALS)' with sqlite header file version $(SQLITE_VERSION)"

win32: CC = i686-w64-mingw32-gcc
win32: win

win64: CC = x86_64-w64-mingw32-gcc
win64: win

win: CFLAGS        += -ffunction-sections -fdata-sections
win: LFLAGS        += -static -Wl,--subsystem,windows,--gc-sections
win: REGEXP_TARGET := $(REGEXP_TARGET).dll
win: UUID_TARGET   := $(UUID_TARGET).dll
win: UDP_TARGET    := $(UDP_TARGET).dll

cygwin unix: CC             = gcc
cygwin unix: CFLAGS        += -fPIC
cygwin unix: REGEXP_TARGET := $(REGEXP_TARGET).so
cygwin unix: UUID_TARGET   := $(UUID_TARGET).so
cygwin unix: UDP_TARGET    := $(UDP_TARGET).so


win cygwin unix: all


regexp: $(REGEXP_TARGET)
	@echo "Built $(REGEXP_TARGET)"

$(REGEXP_TARGET): $(REGEXP_SRC)
	$(call mkdir, $(@D))
	$(CC) -o $(REGEXP_TARGET) $(CFLAGS) $(REGEXP_SRC) $(LFLAGS)

uuid: $(UUID_TARGET)
	@echo "Built $(UUID_TARGET)"

$(UUID_TARGET): $(UUID_SRC)
	$(call mkdir, $(@D))
	$(CC) -o $(UUID_TARGET) $(CFLAGS) $(UUID_SRC) $(LFLAGS)

udp: $(UDP_TARGET)
	@echo "Built $(UDP_TARGET)"

$(UDP_TARGET): $(UDP_SRC)
	$(call mkdir, $(@D))
	$(CC) -o $(UDP_TARGET) $(CFLAGS) $(UDP_SRC) $(LFLAGS) -lWs2_32

clean:
	$(call rmdir, bin)

ls-os:
	@echo $(SUPPORTED_OS)
