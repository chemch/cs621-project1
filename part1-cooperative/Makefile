## setting the compiler to gcc
CC = gcc

## setting compiler flags: -Wall enables all warnings, -Wextra enables additional warnings, -std=c11 uses the C11 standard
CFLAGS = -Wall -Wextra -std=c11

## source files to compile to object files
SRC = compdetect_client.c cJSON.c

## automatically convert .c files to .o files
OBJ = $(SRC:.c=.o)

## executable name to generate
TARGET = compdetect_client

## by default, build the executable
all: $(TARGET)

## link object files into the final executable
$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ)

## convert each .c file into a .o file
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

## remove all object files and the compiled executable
clean:
	rm -f $(OBJ) $(TARGET)
