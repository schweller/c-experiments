# AI Generated
CC = gcc
# Why these flags?
# -Wall: Enable all warnings (catches common mistakes)
# -Wextra: Enable extra warnings (even more safety)
# -g: Include debug symbols (allows debugging with gdb)
# -std=c99: Use C99 standard (good balance of features and compatibility)
CFLAGS = -Wall -Wextra -g -std=c99

# Libraries to link
# -lncurses: Link the ncurses library (required for terminal UI)
LDFLAGS = -lncurses

# Target executable name
TARGET = menu

# Source files (add more .c files here as your project grows)
SOURCES = menu.c

# Object files (automatically derived from sources)
# This substitution replaces .c with .o
OBJECTS = $(SOURCES:.c=.o)

# Default target - runs when you type just "make"
# The first target in a Makefile is the default
all: $(TARGET)

# How to build the final executable
# $@ means "the target" (menu)
# $^ means "all prerequisites" (menu.o)
$(TARGET): $(OBJECTS)
	@echo "Linking $@..."
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS)
	@echo "Build complete! Run with: ./$(TARGET)"

# Pattern rule: how to build .o files from .c files
# $< means "the first prerequisite" (the .c file)
# This automatically creates menu.o from menu.c
%.o: %.c
	@echo "Compiling $<..."
	$(CC) $(CFLAGS) -c $< -o $@

# Run the program (type "make run")
run: $(TARGET)
	./$(TARGET)

# Clean up compiled files (type "make clean")
# -f flag prevents errors if files don't exist
# @ prefix suppresses command echo
clean:
	@echo "Cleaning up..."
	@rm -f $(OBJECTS) $(TARGET)
	@echo "Clean complete!"

# Deep clean - also removes backup files and core dumps
distclean: clean
	@rm -f *~ core

# Install ncurses if needed (Ubuntu/Debian)
# Type "make install-deps" to install dependencies
install-deps:
	@echo "Installing ncurses development library..."
	@sudo apt-get update
	@sudo apt-get install -y libncurses5-dev libncursesw5-dev

# For macOS users (Homebrew)
install-deps-mac:
	@echo "Installing ncurses via Homebrew..."
	@brew install ncurses

# Show debug info (useful for troubleshooting)
debug:
	@echo "Sources: $(SOURCES)"
	@echo "Objects: $(OBJECTS)"
	@echo "Target: $(TARGET)"
	@echo "Compiler: $(CC)"
	@echo "CFLAGS: $(CFLAGS)"
	@echo "LDFLAGS: $(LDFLAGS)"

# Phony targets - these don't represent actual files
# Prevents conflicts if files named "clean" or "run" exist
.PHONY: all clean distclean run install-deps install-deps-mac debug