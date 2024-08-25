# Directories
SRC_DIR := src
BUILD_DIR := build
BIN_DIR := bin
INCLUDE_DIR := include

# Compiler and flags
CC := gcc
CFLAGS := -Wall -Wextra -I$(INCLUDE_DIR) -fopenmp -lgmp -lncurses -lpthread

# Source files
SRCS := $(wildcard $(SRC_DIR)/*.c)
# Object files
OBJS := $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(SRCS))

# Executable name
TARGET := $(BIN_DIR)/perfectNumberFinder

# Default target
all: $(TARGET)

# Link the executable
$(TARGET): $(OBJS)
	@mkdir -p $(BIN_DIR)
	$(CC) $(OBJS) -o $@ $(CFLAGS)

# Compile source files to object files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Run the program
run: $(TARGET)
	./$(TARGET)

# Clean up build and bin directories
clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)

.PHONY: all clean run