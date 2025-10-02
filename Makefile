# Makefile
CC=clang
CFLAGS=-lcrypto -lssl -lm -D_DEFAULT_SOURCE -O2 -Wall -pedantic -std=c99
ARGON=libargon2/libargon2.a
MAC=-I/opt/homebrew/opt/openssl/include -L/opt/homebrew/opt/openssl/lib
manager:
	$(CC) manager.c storage.c $(ARGON) -Isrc $(MAC) $(CFLAGS) -o manager
clean:
	rm manager
