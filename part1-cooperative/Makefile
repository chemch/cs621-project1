# compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -Ijson -Iconfiguration -Itransport -Iapp -I.

# directories for binaries and object files
BIN_DIR = bin
BUILD_DIR = build

# source files
SRC_CLIENT = app/compdetect_client.c json/cJSON.c configuration/cConfig.c transport/cUDP.c
SRC_SERVER = app/compdetect_server.c json/cJSON.c configuration/cConfig.c

# object file definitions (stored in build/)
OBJ_CLIENT = $(patsubst %.c, $(BUILD_DIR)/%.o, $(SRC_CLIENT))
OBJ_SERVER = $(patsubst %.c, $(BUILD_DIR)/%.o, $(SRC_SERVER))

# output binaries (stored in bin/)
TARGET_CLIENT = $(BIN_DIR)/compdetect_client
TARGET_SERVER = $(BIN_DIR)/compdetect_server

# default target
all: $(BIN_DIR) $(BUILD_DIR) $(TARGET_CLIENT) $(TARGET_SERVER)

# create directories
$(BIN_DIR) $(BUILD_DIR):
	mkdir -p $@

# build client binary
$(TARGET_CLIENT): $(OBJ_CLIENT)
	$(CC) $(CFLAGS) -o $@ $^

# suild server binary
$(TARGET_SERVER): $(OBJ_SERVER)
	$(CC) $(CFLAGS) -o $@ $^

# compile .c files to .o files
$(BUILD_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# clean up directive
clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)
