CC = gcc
CFLAGS = -Iinclude -Wall -Wextra -g
LDFLAGS = -Llib

SRC_DIR = src
LIB_DIR = lib
BUILD_DIR = build
BIN_DIR = bin
TARGET = $(BIN_DIR)/c_webserver

# Source files
SRCS = $(shell find $(SRC_DIR) -name '*.c')
LIB_SRCS = $(shell find $(LIB_DIR) -name '*.c')

# Object files
OBJS = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRCS))
LIB_OBJS = $(patsubst $(LIB_DIR)/%.c,$(BUILD_DIR)/lib/%.o,$(LIB_SRCS))

.PHONY: all clean directories

all: directories $(TARGET)

directories:
	mkdir -p $(BIN_DIR)
	mkdir -p $(BUILD_DIR)
	mkdir -p $(BUILD_DIR)/lib

# Compile src
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# Compile lib
$(BUILD_DIR)/lib/%.o: $(LIB_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# Link everything together
$(TARGET): $(OBJS) $(LIB_OBJS)
	$(CC) $^ -o $@ $(LDFLAGS)

clean:
	rm -rf $(BUILD_DIR)/*
	rm -f $(TARGET)

