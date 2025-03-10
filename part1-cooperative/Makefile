# Compiler and Flags
CC = gcc
CFLAGS = -Wall -Wextra -std=c11

# Source files for client and server
SRC_CLIENT = compdetect_client.c cJSON.c
SRC_SERVER = compdetect_server.c cJSON.c

# Object files
OBJ_CLIENT = $(SRC_CLIENT:.c=.o)
OBJ_SERVER = $(SRC_SERVER:.c=.o)

# Output binaries
TARGET_CLIENT = compdetect_client
TARGET_SERVER = compdetect_server

# Default target: Build both client and server
all: $(TARGET_CLIENT) $(TARGET_SERVER)

# Build client
$(TARGET_CLIENT): $(OBJ_CLIENT)
	$(CC) $(CFLAGS) -o $(TARGET_CLIENT) $(OBJ_CLIENT)

# Build server
$(TARGET_SERVER): $(OBJ_SERVER)
	$(CC) $(CFLAGS) -o $(TARGET_SERVER) $(OBJ_SERVER)

# Compile .c files to .o files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up compiled files
clean:
	rm -f $(OBJ_CLIENT) $(TARGET_CLIENT) $(OBJ_SERVER) $(TARGET_SERVER)
