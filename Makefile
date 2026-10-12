# Compilateur et options
CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -Iinclude
LDFLAGS = -lssl -lcrypto

# Dossiers
SRC_DIR = src
BUILD_DIR = build

# Nom de l'exécutable imposé par le sujet (à la racine du dépôt)
TARGET = t3c

# Recherche automatique de tous les fichiers .c dans le dossier src/
SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(SRCS))

# Règle principale
all: $(TARGET)

# Édition de liens pour créer l'exécutable t3c à la racine
$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET) $(LDFLAGS)

# Compilation de chaque fichier .c en .o dans le dossier build/
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Création automatique du dossier build si besoin
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Nettoyage des fichiers compilés et de l'exécutable
clean:
	rm -rf $(BUILD_DIR) $(TARGET)

# Évite les conflits si des fichiers s'appellent "all" ou "clean"
.PHONY: all clean