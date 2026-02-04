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
TARGET = collections_demo
BUILD_DIR = build

# Dynamically find all source files in scripts/ (shared library)
SCRIPT_SRCS := $(call rwildcard,scripts,*.c)
SCRIPT_OBJS := $(addprefix $(BUILD_DIR)/,$(notdir $(SCRIPT_SRCS:.c=.o)))

# Demo source file
DEMO_MAIN := collections_demo.c
DEMO_MAIN_OBJ := $(BUILD_DIR)/collections_demo.o

# Add source directories to vpath so Make can find them
SCRIPT_DIRS := $(sort $(dir $(SCRIPT_SRCS)))
vpath %.c $(SCRIPT_DIRS) .

# =========================================================
# Build Targets
# =========================================================

all: $(BUILD_DIR) $(TARGET)
	@echo ""
	@echo "✨ Build complete! Your executable is ready:"
	@echo "   ./$(TARGET) - Complete collections + iterators demo"
	@echo ""

$(BUILD_DIR):
	@echo "📁 Creating build directory..."
	@mkdir -p $(BUILD_DIR)

# Link demo (shared objects + demo main)
$(TARGET): $(SCRIPT_OBJS) $(DEMO_MAIN_OBJ)
	@echo ""
	@echo "🔗 Linking collections demo..."
	@$(CC) $(CFLAGS) -o $@ $^
	@echo "✅ Successfully created $(TARGET)"

# Compile shared library object files
$(BUILD_DIR)/%.o: %.c | $(BUILD_DIR)
	@echo "🔨 Compiling $< → $(notdir $@)"
	@$(CC) $(CFLAGS) -c $< -o $@

# Compile demo main file
$(DEMO_MAIN_OBJ): $(DEMO_MAIN) | $(BUILD_DIR)
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
	@echo "  make       - Build the demo executable"
	@echo "  make demo  - Build and run the complete demo"
	@echo "  make clean - Remove all build artifacts"
	@echo "  make rebuild - Clean and rebuild from scratch"
	@echo "  make help  - Show this help message"
	@echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
	@echo ""

.PHONY: all demo clean rebuild help
