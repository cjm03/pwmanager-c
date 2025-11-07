# Makefile
CC=gcc
CFLAGS=-g -O2 -Wall -pedantic -I/include -lssl -lcrypto -std=c99
TARGET=pwm
SRC=src/manager.c src/pwgen.c src/storage.c src/utility.c

#Default Target
all: clean build

pwgen:
	$(CC) src/cli-gen.c src/pwgen.c $(CFLAGS) -o pwgen

test:
	$(CC) src/test.c src/pwgen.c src/storage.c src/utility.c $(CFLAGS) -o test

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
