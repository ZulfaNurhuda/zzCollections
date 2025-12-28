CC = gcc
# Recursive wildcard function to find files in subdirectories
rwildcard=$(foreach d,$(wildcard $(1:=/*)),$(call rwildcard,$d,$2) $(filter $(subst *,%,$2),$d))

# Dynamically find all header directories
HEADER_DIRS := $(sort $(dir $(call rwildcard,headers,*.h)))
INCLUDES := $(addprefix -I,$(HEADER_DIRS))

CFLAGS = -std=c11 -Wall -Wextra -Wpedantic -O2 -g $(INCLUDES)
TARGET = demo.exe
BUILD_DIR = build

# Dynamically find all source files in scripts/ and demo.c
SCRIPT_SRCS := $(call rwildcard,scripts,*.c)
SRCS := demo.c $(SCRIPT_SRCS)

# Create object filenames (flatten paths to build directory)
OBJS := $(addprefix $(BUILD_DIR)/,$(notdir $(SRCS:.c=.o)))

# Add source directories to vpath so Make can find them
SCRIPT_DIRS := $(sort $(dir $(SCRIPT_SRCS)))
vpath %.c $(SCRIPT_DIRS) .

all: $(BUILD_DIR) $(TARGET)

$(BUILD_DIR):
	@mkdir -p $(BUILD_DIR)

$(TARGET): $(OBJS)
	@echo "🔗 Linking $(TARGET)..."
	@$(CC) $(CFLAGS) -o $@ $^
	@echo "✓ Build successful! Executable: $(TARGET)"

# Generic pattern rule for compiling object files
# vpath will locate the source file for us
$(BUILD_DIR)/%.o: %.c | $(BUILD_DIR)
	@echo "🔨 Compiling $<..."
	@$(CC) $(CFLAGS) -c $< -o $@

demo: $(TARGET)
	@echo "Running demo..."
	@./$(TARGET)

clean:
	rm -rf $(BUILD_DIR) $(TARGET)
	@echo "✓ Cleaned!"

rebuild: clean all

.PHONY: all demo clean rebuild
