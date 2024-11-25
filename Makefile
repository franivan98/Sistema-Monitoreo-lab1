# Variables
CC = gcc
CFLAGS = -I include
SRC = src/expose_metrics.c src/metrics.c src/main.c
TARGET = build/metrics
LIBS = -lprom -pthread -lpromhttp

# Regla por defecto
all: $(TARGET)

# Regla para construir el ejecutable
$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $@ $(LIBS)

# Limpiar archivos generados
clean:
	rm -f $(TARGET)

.PHONY: all clean
