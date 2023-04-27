PROJECTS   = regexp uuid udp

REGEXP_SRC = $(wildcard regexp/*.c)
UUID_SRC   = $(wildcard uuid/*.c)
UDP_SRC    = $(wildcard udp/*.c)

OUT_DIR    = bin

REGEXP_TARGET = regexp
UUID_TARGET   = uuid
UDP_TARGET    = udp

# detect the current dev environment
ifeq '$(findstring ;,$(PATH))' ';'
    UNAME := Windows
	mkdir = @mkdir $(subst /,\,$(1)) >nul 2>&1 || (exit 0)
else
    UNAME := $(shell uname 2>/dev/null || echo Unknown)
    UNAME := $(patsubst CYGWIN%,Cygwin,$(UNAME))
    UNAME := $(patsubst MSYS%,MSYS,$(UNAME))
    UNAME := $(patsubst MINGW%,MSYS,$(UNAME))
	mkdir = @mkdir -p $(1)
endif

# Default targeting operating system (os).
ifeq ($(UNAME),Windows)
SUPPORTED_OS = win32 win64
else ifeq ($(UNAME),Cygwin)
SUPPORTED_OS = cygwin win32 win64
else
SUPPORTED_OS = unix
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
endif

# Prepend directory to target files
REGEXP_TARGET := $(OUT_DIR)/$(MAKECMDGOALS)/$(REGEXP_TARGET)
UUID_TARGET   := $(OUT_DIR)/$(MAKECMDGOALS)/$(UUID_TARGET)
UDP_TARGET    := $(OUT_DIR)/$(MAKECMDGOALS)/$(UDP_TARGET)

all: $(PROJECTS)
	@echo "Built for os '$(MAKECMDGOALS)' with sqlite header file version 3.36.0."

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
	rm -rf bin

ls-os:
	@echo $(SUPPORTED_OS)
