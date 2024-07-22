# Nom de la bibliothèque
LIBRARY_NAME = libpixelfactory.a

# Dossiers
SRC_DIR = src
INC_DIR = include
OBJ_DIR = obj

# Fichiers source et objets
SOURCES = $(shell find $(SRC_DIR) -name '*.c')
OBJECTS = $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SOURCES))

# Récupérer les répertoires des fichiers objets
OBJ_DIRS = $(sort $(dir $(OBJECTS)))

# Compilateur et options
CC = gcc
CFLAGS = -I$(INC_DIR) -Wall -Wextra -O3 -lm -fopenmp -mavx2
AR = ar
ARFLAGS = rcs

# Règles
.PHONY: all clean

all: $(LIBRARY_NAME)

$(LIBRARY_NAME): $(OBJECTS)
	$(AR) $(ARFLAGS) $@ $^

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIRS)
	$(CC) $(CFLAGS) -c $< -o $@

# Créer les répertoires nécessaires dans OBJ_DIR
$(OBJ_DIRS):
	mkdir -p $@

clean:
	rm -rf $(OBJ_DIR) $(LIBRARY_NAME)
