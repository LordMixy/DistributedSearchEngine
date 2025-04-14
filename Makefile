# Makefile per progetto C senza cartella include
# Struttura:
#   myproject/
#     src/      (file .c e .h)
#     build/    (file oggetto e eseguibili)

# Compilatore e flag
CC = gcc
CFLAGS = -Wall -Wextra
LDFLAGS = 
TARGET = build/dse

# Strutture directory
SRC_DIR = src
BUILD_DIR = build
SRC = $(wildcard $(SRC_DIR)/*.c)
OBJ = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRC))
DEP = $(OBJ:.o=.d)

# Regole di compilazione
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -MMD -c $< -o $@

$(TARGET): $(OBJ)
	$(CC) $(LDFLAGS) $^ -o $@

# Include le dipendenze
-include $(DEP)

# Regole utilità
.PHONY: all clean

all: $(TARGET)

clean:
	rm -rf $(BUILD_DIR)

# Output colorato (opzionale)
ifndef V
CC_COLOR = \033[34m
BIN_COLOR = \033[37;1m
END_COLOR = \033[0m
QUIET_CC = @printf '  %b %b\n' $(CC_COLOR)CC$(END_COLOR) $(BIN_COLOR)$@$(END_COLOR);
QUIET_LINK = @printf '  %b %b\n' $(CC_COLOR)LINK$(END_COLOR) $(BIN_COLOR)$@$(END_COLOR);
endif
