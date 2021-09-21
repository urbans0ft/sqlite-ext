PROJECTS   = regexp uuid udp

REGEXP_SRC = $(wildcard regexp/*.c)
UUID_SRC   = $(wildcard uuid/*.c)
UDP_SRC    = $(wildcard udp/*.c)

OUT_DIR    = bin

REGEXP_TARGET = regexp.so
UUID_TARGET   = uuid.so
UDP_TARGET    = udp.so

# Default targeting operating system (os).
TARGET_OS    = CYG64
SUPPORTED_OS = CYG64 UNIX64 WIN64 WIN32

# Error if targeting os is not in supported os list
ifeq (,$(findstring $(TARGET_OS),$(SUPPORTED_OS)))
$(error Target OS '$(TARGET_OS)' is not supported.)
endif

# Differentiate between Windows and none Windows
ifneq (,$(findstring WIN,$(TARGET_OS)))
ifneq (,$(findstring 64,$(TARGET_OS)))
CC             = x86_64-w64-mingw32-gcc
else
CC             = i686-w64-mingw32-gcc
endif
CFLAGS         = -Isqlite -Wall -ffunction-sections -fdata-sections
LFLAGS         = -s -shared -static -Wl,--subsystem,windows,--gc-sections
REGEXP_TARGET := $(REGEXP_TARGET:.so=.dll)
UUID_TARGET   := $(UUID_TARGET:.so=.dll)
UDP_TARGET    := $(UDP_TARGET:.so=.dll)
else
CC             = gcc
CFLAGS         = -g -fPIC -shared -Isqlite -Wall
endif

# Prepend directory to target files
REGEXP_TARGET := $(OUT_DIR)/$(TARGET_OS)/$(REGEXP_TARGET)
UUID_TARGET   := $(OUT_DIR)/$(TARGET_OS)/$(UUID_TARGET)
UDP_TARGET   := $(OUT_DIR)/$(TARGET_OS)/$(UDP_TARGET)

all: $(PROJECTS)
	@echo "Built for os '$(TARGET_OS)' with sqlite header file version 3.36.0."

regexp: $(REGEXP_TARGET)
	@echo "Built $(REGEXP_TARGET)"

$(REGEXP_TARGET): $(REGEXP_SRC)
	@mkdir -p $(@D)
	$(CC) -o $(REGEXP_TARGET) $(CFLAGS) $(REGEXP_SRC)

uuid: $(UUID_TARGET)
	@echo "Built $(UUID_TARGET)"

$(UUID_TARGET): $(UUID_SRC)
	@mkdir -p $(@D)
	$(CC) -o $(UUID_TARGET) $(CFLAGS) $(UUID_SRC)

udp: $(UDP_TARGET)
	@echo "Built $(UDP_TARGET)"

$(UDP_TARGET): $(UDP_SRC)
	@mkdir -p $(@D)
	$(CC) -o $(UDP_TARGET) $(CFLAGS) $(UDP_SRC) $(LFLAGS) -lWs2_32

clean:
	rm -rf bin

ls-os:
	@echo $(SUPPORTED_OS)
