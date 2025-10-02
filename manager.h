#ifndef MANAGER_H
#define MANAGER_H

#include <stdint.h>

int uGenSalt(uint8_t* buffer, int bytes);

char* genSalt(int bytes);

void hashArgon(char* password, uint8_t* hashbuf, uint8_t* salt);

void encodedHashArgon(char* password, char* encoded, uint8_t* salt);

void verifyEncodedArgon(char* encoded);

char* verifyArgon(uint8_t* password, uint8_t* salt);

#endif // MANAGER_H
