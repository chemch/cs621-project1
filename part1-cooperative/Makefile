# Compiler and Flags
CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -Ijson -Iconfiguration -Itransport -Iapp -I.

BIN_DIR = bin
BUILD_DIR = build

# Source files
SRC_CLIENT = app/compdetect_client.c json/cJSON.c configuration/cConfig.c transport/cUDP.c
SRC_SERVER = app/compdetect_server.c json/cJSON.c configuration/cConfig.c

# Object files (stored in build/)
OBJ_CLIENT = $(patsubst %.c, $(BUILD_DIR)/%.o, $(SRC_CLIENT))
OBJ_SERVER = $(patsubst %.c, $(BUILD_DIR)/%.o, $(SRC_SERVER))

# Output binaries (stored in bin/)
TARGET_CLIENT = $(BIN_DIR)/compdetect_client
TARGET_SERVER = $(BIN_DIR)/compdetect_server

# Default target
all: $(BIN_DIR) $(BUILD_DIR) $(TARGET_CLIENT) $(TARGET_SERVER)

# Create directories
$(BIN_DIR) $(BUILD_DIR):
	mkdir -p $@

# Build client binary
$(TARGET_CLIENT): $(OBJ_CLIENT)
	$(CC) $(CFLAGS) -o $@ $^

# Build server binary
$(TARGET_SERVER): $(OBJ_SERVER)
	$(CC) $(CFLAGS) -o $@ $^

# Compile .c files to .o files in build/
$(BUILD_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up
clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)