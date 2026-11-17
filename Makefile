# Simple Makefile for DSi cyberdeck project

# ----- Toolchain -----
export DEVKITARM   := $(DEVKITPRO)/devkitARM
export PATH        := $(DEVKITARM)/bin:$(PATH)

# ----- Project name -----
TARGET := cyberdeck

# ----- Source files -----
SRCS := main.c

# ----- Build flags -----
CC := $(DEVKITARM)/bin/arm-none-eabi-gcc
OBJCOPY := $(DEVKITARM)/bin/arm-none-eabi-objcopy
CFLAGS := -Wall -O2 -mthumb -mtune=arm7 -I$(DEVKITARM)/include -I.
LDFLAGS := -specs=ds_arm9.specs

# ----- Output -----
BIN := $(TARGET).elf
NDS := $(TARGET).nds

all: $(NDS)

$(BIN): $(SRCS)
	$(CC) $(CFLAGS) -o $@ $^

$(NDS): $(BIN)
	$(OBJCOPY) -O binary $< $@

clean:
	rm -f $(BIN) $(NDS)
