# =========================================================
# zzCollections Library - Build Configuration
# =========================================================

CC = gcc

# Recursive wildcard function to find files in subdirectories
rwildcard=$(foreach d,$(wildcard $(1:=/*)),$(call rwildcard,$d,$2) $(filter $(subst *,%,$2),$d))

# Dynamically find all header directories
HEADER_DIRS := $(sort $(dir $(call rwildcard,headers,*.h)))
INCLUDES := $(addprefix -I,$(HEADER_DIRS))

# Compiler flags for C11 with warnings and optimizations
CFLAGS = -std=c11 -Wall -Wextra -Wpedantic -O2 -g -pipe $(INCLUDES)

# Build output
TARGET = collection_demo
BUILD_DIR = build

# Dynamically find all source files in scripts/ and demo.c
SCRIPT_SRCS := $(call rwildcard,scripts,*.c)
SRCS := demo.c $(SCRIPT_SRCS)

# Create object filenames (flatten paths to build directory)
OBJS := $(addprefix $(BUILD_DIR)/,$(notdir $(SRCS:.c=.o)))

# Add source directories to vpath so Make can find them
SCRIPT_DIRS := $(sort $(dir $(SCRIPT_SRCS)))
vpath %.c $(SCRIPT_DIRS) .

# =========================================================
# Build Targets
# =========================================================

all: $(BUILD_DIR) $(TARGET)
	@echo ""
	@echo "✨ Build complete! Your executable is ready: ./$(TARGET)"
	@echo "   Run './$(TARGET)' to see all 15 data structures in action!"
	@echo ""

$(BUILD_DIR):
	@echo "📁 Creating build directory..."
	@mkdir -p $(BUILD_DIR)

$(TARGET): $(OBJS)
	@echo ""
	@echo "🔗 Linking object files into executable..."
	@TMP=$(BUILD_DIR) TEMP=$(BUILD_DIR) $(CC) $(CFLAGS) -o $@ $^
	@echo "✅ Successfully created $(TARGET)"

# Generic pattern rule for compiling object files
# vpath will locate the source file for us
$(BUILD_DIR)/%.o: %.c | $(BUILD_DIR)
	@echo "🔨 Compiling $< → $(notdir $@)"
	@$(CC) $(CFLAGS) -c $< -o $@

# =========================================================
# Utility Targets
# =========================================================

demo: $(TARGET)
	@echo ""
	@echo "🚀 Running zzCollections demo..."
	@echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
	@./$(TARGET)
	@echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
	@echo "✨ Demo completed successfully!"
	@echo ""

checkmem: $(TARGET)
	@echo ""
	@echo "🔍 Running memory leak detection with Valgrind..."
	@echo "   This will check for memory leaks, invalid reads/writes, and more."
	@echo ""
	@valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./$(TARGET)
	@echo ""
	@echo "✅ Memory check complete!"
	@echo ""

clean:
	@echo "🧹 Cleaning up build artifacts..."
	@rm -rf $(BUILD_DIR) $(TARGET)
	@echo "✨ All clean! Build directory and executable removed."
	@echo ""

rebuild: clean all
	@echo "🔄 Rebuild complete!"

help:
	@echo ""
	@echo "zzCollections - Available Make Targets"
	@echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
	@echo "  make          - Build the demo executable"
	@echo "  make demo     - Build and run the demo program"
	@echo "  make checkmem - Run Valgrind memory leak detection"
	@echo "  make clean    - Remove all build artifacts"
	@echo "  make rebuild  - Clean and rebuild from scratch"
	@echo "  make help     - Show this help message"
	@echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
	@echo ""

.PHONY: all demo checkmem clean rebuild help
