# -------------------------
# VARIABLES
# -------------------------
# PROJECT
TARGET := domus
VERSION := 1.0.0
LICENSE := MIT
SLOGAN := Unicuique sua domus nota
DESCRIPTION := Home Automation at your CLI
# DIRECTORIES
SRC_DIR := ./src
HDR_DIR := ./include
BUILD_DIR := ./build
OBJ_DIR := $(BUILD_DIR)/obj
BIN_DIR := $(BUILD_DIR)/bin
# SOURCES & OBJECTS
SRC := $(shell find $(SRC_DIR)/ -type f -name '*.c')
OBJ := $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRC))
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

$(TARGET): $(OBJ)
	$(MKDIR) $(BIN_DIR)
	$(CC) $^ -o $(BIN_DIR)/$@

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