REGEXP_SRC = $(wildcard regexp/*.c)
UUID_SRC   = $(wildcard uuid/*.c)

OUT_DIR    = bin

REGEXP_TARGET = regexp.so
UUID_TARGET   = uuid.so

# Default targeting operating system (os).
TARGET_OS    = CYG64
SUPPORTED_OS = CYG64 UNIX64 WIN64

# Error if targeting os is not in supported os list
ifeq (,$(findstring $(TARGET_OS),$(SUPPORTED_OS)))
$(error Target OS '$(TARGET_OS)' is not supported.)
endif

# Differentiate between Windows and none Windows
ifneq (,$(findstring WIN,$(TARGET_OS)))
CC             = x86_64-w64-mingw32-gcc
CFLAGS         = -g -shared -Isqlite
REGEXP_TARGET := $(REGEXP_TARGET:.so=.dll)
UUID_TARGET   := $(UUID_TARGET:.so=.dll)
else
CC             = gcc
CFLAGS         = -g -fPIC -shared -Isqlite
endif

# Prepend directory to target files
REGEXP_TARGET := $(OUT_DIR)/$(TARGET_OS)/$(REGEXP_TARGET)
UUID_TARGET   := $(OUT_DIR)/$(TARGET_OS)/$(UUID_TARGET)

all: regexp uuid
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

clean:
	rm -rf bin

ls-os:
	@echo $(SUPPORTED_OS)
