# Makefile
CC=gcc
CFLAGS=-g -O2 -Wall -pedantic -lssl -lcrypto -std=c99
pwgen:
	$(CC) main.c pwgen.c $(CFLAGS) -o pwgen
test:
	$(CC) test.c pwgen.c storage.c $(CFLAGS) -o test
clean:
	rm pwgen
