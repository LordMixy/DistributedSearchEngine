# Cross-platform Makefile for C project without include folder
# Structure:
#   myproject/
#     src/      (C and H files)
#     build/    (object files and executables)

# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -Wshadow -Wstrict-prototypes -Wdouble-promotion -Wformat=2 -Wpedantic -ggdb -DDEBUG # -O3 -march=native -fomit-frame-pointer
LDFLAGS = -pg

# Directory structures
SRC_DIR = src
BUILD_DIR = build

# Handle platform differences
ifeq ($(OS),Windows_NT)
    # Windows-specific settings
    TARGET = build/nirvana.exe
    MKDIR = mkdir $(subst /,\,$(BUILD_DIR))
    RM = rmdir /s /q $(subst /,\,$(BUILD_DIR))
else
    # Unix-specific settings
    TARGET = build/nirvana
    MKDIR = mkdir -p $(BUILD_DIR)
    RM = rm -rf $(BUILD_DIR)
endif

# Find source files
SRC = $(wildcard $(SRC_DIR)/*.c)
OBJ = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRC))
DEP = $(OBJ:.o=.d)

# Default target
.PHONY: all
all: $(TARGET)

# Rule to create build directory
$(BUILD_DIR):
	$(MKDIR)

# Compilation rule - note the tab indentation and single command per line
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -MMD -c $< -o $@

# Linking rule
$(TARGET): $(OBJ)
	$(CC) $(LDFLAGS) $^ -o $@

# Include dependencies
-include $(DEP)

# Clean rule
.PHONY: clean
clean:
	$(RM)
