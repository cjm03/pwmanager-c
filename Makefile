# Makefile
CC=gcc
CFLAGS=-g -O2 -Wall -pedantic -lssl -lcrypto -std=c99
TARGET=pwm
SRC=manager.c pwgen.c storage.c utility.c

#Default Target
all: clean build

pwgen:
	$(CC) cli-gen.c pwgen.c $(CFLAGS) -o pwgen

test:
	$(CC) test.c pwgen.c storage.c utility.c $(CFLAGS) -o test

build:
	@echo "Compiling $(TARGET)..."
	$(CC) $(SRC) $(CFLAGS) -o $(TARGET)
	@echo "Build complete"

clean:
	@echo "Checking for existing binary..."
	@if [ -f $(TARGET) ]; then \
		echo "Removing old binary $(TARGET)..."; \
		rm -f $(TARGET); \
	else \
		echo "No existing binary found"; \
	fi
