# VARIABLES
TARGET = domus

CC = gcc
CFLAGS = -std=gnu90 -Wall -g

SRC_DIR = src
HEADER_DIR = include
BUILD_DIR = build
SRC := $(shell find $(SRC_DIR) -name '*.c')
OBJ := $(addprefix $(BUILD_DIR)/,$(SRC:%.c=%.o))

ECHO = echo
RM = rm -rf
# END VARIABLES

.PHONY: all clean setup

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(TARGET)
$(BUILD_DIR)/%.o: %.c
	$(CC) $(CFLAGS) -I$(HEADER_DIR) -I$(dir $<) -c $< -o $@
help:
	@$(ECHO) "all    -  build & compile what is necessary"
	@$(ECHO) "clean  -  cleanup old .o and binaries"
	@$(ECHO) "help   -  Show useful information"
build:

clean:
	$(RM) $(OBJ) $(TARGET)