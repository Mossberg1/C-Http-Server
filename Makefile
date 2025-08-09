CC = gcc
CFLAGS = -Iinclude -Wall -Wextra -g
LDFLAGS =

SRC_DIR = src
BUILD_DIR = build
BIN_DIR = bin
TARGET = $(BIN_DIR)/c_webserver

# Find all .c files recursively in src
SRCS = $(shell find $(SRC_DIR) -name '*.c')

# Map source files to object files in build directory, preserving directory structure
OBJS = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRCS))

.PHONY: all clean directories

all: directories $(TARGET)

directories:
	mkdir -p $(BIN_DIR)
	mkdir -p $(BUILD_DIR)

# Compile .c to .o in build, create needed subdirs
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# Link objects to executable
$(TARGET): $(OBJS)
	$(CC) $(LDFLAGS) $^ -o $@

clean:
	rm -rf $(BUILD_DIR)/*
	rm -f $(TARGET)
