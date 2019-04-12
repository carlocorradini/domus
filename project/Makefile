# VARIABLES
TARGET = domus


SRC_DIR = src
HEADER_DIR = include
BUILD_DIR = build
SRC := $(shell find $(SRC_DIR)/ -type f -name '*.c')
OBJ := $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(SRC))

CC = gcc
CFLAGS = -std=gnu90 -Wall -g
CPPFLAGS = -I$(HEADER_DIR)


ECHO = echo
RM = rm -rf
# END VARIABLES

.PHONY: all clean setup

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $^ -o $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

help:
	@$(ECHO) $(SRC)
	@$(ECHO) $(OBJ)
	@$(ECHO) "all    -  build & compile what is necessary"
	@$(ECHO) "clean  -  cleanup old .o and binaries"
	@$(ECHO) "help   -  Show useful information"

build:

clean:
	$(RM) $(BUILD_DIR) $(TARGET)