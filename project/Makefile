# -------------------------
# VARIABLES
# -------------------------
# PROJECT
PRJ_NAME := domus
VERSION := 1.0.0
LICENSE := MIT
SLOGAN := Unicuique sua domus nota
DESCRIPTION := Home Automation at your CLI

# MAINS
TARGET:= main bulb window fridge hub

# DIRECTORIES
SRC_DIR := ./src/src
DEV_SRC_DIR := $(SRC_DIR)/device
HDR_DIR := ./src/include
BUILD_DIR := ./build
OBJ_DIR := $(BUILD_DIR)/obj
DEV_OBJ_DIR := $(OBJ_DIR)/device
BIN_DIR := $(BUILD_DIR)/bin
DEV_BIN := $(BIN_DIR)/device

# SOURCES & OBJECTS
SRC := $(shell find $(SRC_DIR)/ -type f -name '*.c')
OBJ := $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRC))
DEV := $(shell find $(DEV_SRC_DIR) -type f -name '*.c')
# Take all .c file in src/device/, exclude controller.c | device.c | communication.c | child.c
DEV_OBJ := $(patsubst $(DEV_SRC_DIR)/%.c, $(DEV_OBJ_DIR)/%.o, $(DEV))
DEV_OBJ := $(foreach a,$(DEV_OBJ),$(if $(findstring controller,$a),,$a))
DEV_OBJ := $(foreach a,$(DEV_OBJ),$(if $(findstring device.o,$a),,$a))
DEV_OBJ := $(foreach a,$(DEV_OBJ),$(if $(findstring communication,$a),,$a))
DEV_OBJ := $(foreach a,$(DEV_OBJ),$(if $(findstring child,$a),,$a))

# All .o files, both commons and specific ones
ALL_OBJ := $(OBJ)

# Only common .o files
OBJ := $(foreach a,$(OBJ),$(if $(findstring main.o,$a),,$a))
OBJ := $(filter-out $(DEV_OBJ), $(OBJ))

# COMPILER
CC := gcc
CFLAGS := -std=gnu90 -Wall -Wextra -pedantic -g
CPPFLAGS := -I$(HDR_DIR)

# UTILS
MKDIR := mkdir -p
RM := rm -r -f
ECHO := echo
ifeq ($(OS), Windows_NT)
	ECHO += -e
endif

# COLORS
COLOR_RESET := \033[0m
COLOR_RED := \033[0;31m
COLOR_GREEN := \033[0;32m
COLOR_YELLOW := \033[0;33m
COLOR_BLUE := \033[0;34m
COLOR_MAGENTA := \033[0;35m
COLOR_CYAN := \033[0;36m
COLOR_WHITE := \033[0;37m
# -------------------------
# END VARIABLES
# -------------------------

# -------------------------
# RECIPES
# -------------------------
.PHONY: all build help clean

all:
	@$(ECHO) "$(COLOR_RED)Make without any recipe is not allowed$(COLOR_RESET)"
	@$(ECHO) "$(COLOR_RED)Please type 'make help' for more information$(COLOR_RESET)"

build: clean $(TARGET)

$(TARGET): $(ALL_OBJ)
	$(MKDIR) $(DEV_BIN)
	#      get the .o file having $(TARGET) in the name                                    add all common files
	$(CC)  $(filter-out $(foreach a,$(ALL_OBJ),$(if $(findstring $@,$a),,$a)), $(ALL_OBJ)) $(OBJ) -o $(DEV_BIN)/$@
	# if the executable is the main one, move it and rename it
	@if [ $@ = main ]; then \
		mv $(DEV_BIN)/$@ $(BIN_DIR)/$(PRJ_NAME); \
	fi;

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(MKDIR) $(dir $@)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

help:
	@$(ECHO) "$(COLOR_GREEN)=== DOMUS ===$(COLOR_RESET)"
	@$(ECHO) "\t$(SLOGAN)"
	@$(ECHO) "$(COLOR_MAGENTA)- AUTHORS$(COLOR_RESET)"
	@$(ECHO) "\tCarlo Corradini  | carlo.corradini@studenti.unitn.it  | 192451"
	@$(ECHO) "\tSimone Nascivera | simone.nascivera@studenti.unitn.it | 193246"
	@$(ECHO) "$(COLOR_MAGENTA)- DESCRIPTION$(COLOR_RESET)"
	@$(ECHO) "\t$(DESCRIPTION)"
	@$(ECHO) "$(COLOR_MAGENTA)- VERSION$(COLOR_RESET)"
	@$(ECHO) "\t$(VERSION)"
	@$(ECHO) "$(COLOR_MAGENTA)- LICENSE$(COLOR_RESET)"
	@$(ECHO) "\t$(LICENSE)"
	@$(ECHO) "$(COLOR_MAGENTA)- RECIPES AVAILABLE$(COLOR_RESET)"
	@$(ECHO) "\t$(COLOR_YELLOW)build$(COLOR_RESET)            Build & Compile all files under $(SRC_DIR) and generate $(TARGET) binaries"
	@$(ECHO) "\t$(COLOR_YELLOW)clean$(COLOR_RESET)            Delete $(BUILD_DIR) directory and $(TARGET) binaries"
	@$(ECHO) "\t$(COLOR_YELLOW)help$(COLOR_RESET)             Show useful information"

clean:
	$(RM) $(BUILD_DIR)
# -------------------------
# END RECIPES
# -------------------------