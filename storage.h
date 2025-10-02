#ifndef STORAGE_H
#define STORAGE_H

#include <stdlib.h>

#define MAXUSERS 16
#define HT_PRIME1 151
#define HT_PRIME2 163
#define HASHLEN 32
#define SALTLEN 16
#define ENCODEDLEN 108

/* 
 * a single User
 * @param username: a char pointer storing username
 * @param salt: a uint8_t pointer storing the salt
 * @param hash: a uint8_t pointer storing the hash
*/
typedef struct User {
    char* username;
    uint8_t* salt;
    uint8_t* hash;
} User;

/*
 * a table of Users
 * @param capacity: maximum amount of Users
 * @param count: amount of Users currently populated
 * @param users: an array of User pointers
*/
typedef struct uTable {
    size_t capacity;
    size_t count;
    User** users;
} uTable;

int storageHash(const char* s, const int a, const int m);
int storageGetHash(const char* s, const int users, const int attempt);

uTable* createTable(void);
User* createUser(char* username, uint8_t* salt, uint8_t* hash);
void insertUser(uTable* ut, char* username, uint8_t* salt, uint8_t* hash);

User* getUser(uTable* ut, const char* username);
void dumpTable(uTable* ut);

void destroyTable(uTable* ut);
void destroyUser(User* u);

#endif // STORAGE_H
