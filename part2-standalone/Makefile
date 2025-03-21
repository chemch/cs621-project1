# compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -Ip2_json -Ip2_configuration -Ip2_transport -Ip2_app -I.

# directories for binaries and object files
BIN_DIR = bin
BUILD_DIR = build

# source files
SRC_CLIENT = p2_app/compdetect.c p2_json/cJSON.c p2_configuration/cConfig.c p2_transport/cUDP.c

# object file definitions (stored in build/)
OBJ_CLIENT = $(patsubst %.c, $(BUILD_DIR)/%.o, $(SRC_CLIENT))

# output binaries (stored in bin/)
TARGET_CLIENT = $(BIN_DIR)/compdetect

# default target
all: $(BIN_DIR) $(BUILD_DIR) $(TARGET_CLIENT) 

# create directories
$(BIN_DIR) $(BUILD_DIR):
	mkdir -p $@

# build client binary
$(TARGET_CLIENT): $(OBJ_CLIENT)
	$(CC) $(CFLAGS) -o $@ $^

# compile .c files to .o files
$(BUILD_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# clean up directive
clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)
