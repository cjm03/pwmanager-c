/*----------------------------------
 *      storage.c
 *  
 *      Hash Table for user info
------------------------------------*/

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "storage.h"

int storageHash(const char* s, const int a, const int m)
{
    long hash = 0;
    const int len = strlen(s);
    for (int i = 0; i < len; i++) {
        hash +=(long)pow(a, len - (i + 1)) * s[i];
        hash = hash % m;
    }
    return (int)hash;
}

int storageGetHash(const char* s, const int users, const int attempt)
{
    const int hasha = storageHash(s, HT_PRIME1, users);
    const int hashb = storageHash(s, HT_PRIME2, users);
    return (hasha + (attempt * (hashb + 1))) % users;
}

uTable* createTable(void)
{
    uTable* ut = malloc(sizeof(uTable));
    if (ut == NULL) {
        fprintf(stderr, "malloc ut failed\n");
        return NULL;
    }
    ut->capacity = MAXUSERS;
    ut->count = 0;
    ut->users = calloc(ut->capacity, sizeof(User*));
    if (ut->users == NULL) {
        fprintf(stderr, "calloc ut->users failed\n");
        free(ut);
        return NULL;
    }
    return ut;
}

User* createUser(char* username, uint8_t* salt, uint8_t* hash)
{
    User* u = malloc(sizeof(User));
    if (u == NULL) {
        fprintf(stderr, "malloc u failed\n");
        return NULL;
    }
    u->username = strdup(username);
    u->salt = malloc(sizeof(uint8_t) * SALTLEN);
    u->hash = malloc(sizeof(uint8_t) * HASHLEN);
    memcpy(u->salt, salt, SALTLEN);
    memcpy(u->hash, hash, HASHLEN);
    return u;
}

void insertUser(uTable* ut, char* username, uint8_t* salt, uint8_t* hash)
{
    User* new = createUser(username, salt, hash);
    int index = storageGetHash(new->username, ut->capacity, 0);
    User* cur = ut->users[index];
    int i = 1;
    while (cur != NULL) {
        if (strcmp(cur->username, username) == 0) {
            destroyUser(cur);
            ut->users[index] = new;
            return;
        }
        index = storageGetHash(new->username, ut->capacity, i);
        cur = ut->users[index];
        i++;
    }
    ut->users[index] = new;
    ut->count++;
}

User* getUser(uTable* ut, const char* username)
{
    int index = storageGetHash(username, ut->capacity, 0);
    User* cur = ut->users[index];
    int i = 1;
    while (cur != NULL) {
        if (strcmp(cur->username, username) == 0) {
            return cur;
        }
        index = storageGetHash(username, ut->capacity, i);
        cur = ut->users[index];
        i++;
    }
    return NULL;
}

void dumpTable(uTable* ut)
{
    for (int i = 0; i < ut->capacity; ++i) {
        User* cur = ut->users[i];
        if (cur != NULL) {
            printf("[%s]:%s -> %s\n", cur->username, cur->salt, cur->hash);
        }
    }
}

void destroyTable(uTable* ut)
{
    for (size_t i = 0; i < ut->capacity; ++i) {
        User* cur = ut->users[i];
        if (cur != NULL) {
            destroyUser(ut->users[i]);
        }
    }
    free(ut->users);
    free(ut);
}

void destroyUser(User* u)
{
    free(u->username);
    free(u->salt);
    free(u->hash);
    free(u);
}






