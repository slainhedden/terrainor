# Compiler
CC = gcc

# Compiler Flags
CFLAGS = -g -Wall -Wextra -pedantic -I.

# Linker Flags
LDFLAGS = -lGL -lGLU -lGLEW -lSDL2 -lm -lglfw

# Source Files
SRC = main.c noise.c render.c terrain.c utils.c
TEST_SRC = test_main.c noise.c utils.c
TEST_MAIN_SRC = noise.c render.c terrain.c utils.c

# Object Files
OBJ = $(SRC:.c=.o)
TEST_OBJ = $(TEST_SRC:.c=.o)
TEST_MAIN_OBJ = $(TEST_SRC:.c=.o)

# Output Executables
TARGET = terrain_generator
TEST_TARGET = test_noise
TEST_MAIN_TARGET = test_main

# Default target
all: $(TARGET)

# Main target for the terrain generator
$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $(TARGET) $(CFLAGS) $(LDFLAGS)

# Compile test executable for noise functions
test_noise: $(TEST_OBJ)
	$(CC) $(TEST_OBJ) -o $(TEST_TARGET) $(CFLAGS) $(LDFLAGS)

test_main: $(TEST_MAIN_OBJ)
	$(CC) $(TEST_MAIN_OBJ) -o $(TEST_MAIN_TARGET) $(CFLAGS) $(LDFLAGS)

# Compile .c files into .o files
%.o: %.c
	$(CC) -c $< -o $@ $(CFLAGS)

# Clean up generated files
clean:
	rm -f $(OBJ) $(TEST_OBJ) $(TARGET) $(TEST_TARGET)

# Phony targets (not actual files)
.PHONY: all clean test_noise

