CC = mpicc
SRC_DIR = src
BUILD_DIR = build

# List all .c files in source directory
SRCS := $(wildcard $(SRC_DIR)/*.c)

# Create corresponding target names for executables
TARGETS := $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%,$(SRCS))

# Rule to build all targets
all: $(TARGETS)

# Rule to compile each source file into corresponding target
$(BUILD_DIR)/%: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) -O3 -o $@ $<

# Rule to create the build directory if it doesn't exist
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Clean rule
clean:
	rm -rf $(BUILD_DIR)

.PHONY: all clean