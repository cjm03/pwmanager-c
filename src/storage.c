#define _DEFAULT_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <ctype.h>
#include "central.h"


UserCard* createEmptyUserCard(void)
{
    UserCard* uc = malloc(sizeof(UserCard));
    if (!uc) {
        fprintf(stderr, "malloc: error allocating memory for UserCard struct uc\n");
        return NULL;
    }
    uc->service_nickname = malloc(16 * sizeof(char));
    uc->service_website = malloc(32 * sizeof(char));
    uc->username = malloc(16 * sizeof(char));
    uc->password = malloc(64 * sizeof(char));
    return uc;
}

UserCard* createUserCard(char* nickname, char* website, char* username, char* password)
{
    UserCard* uc = malloc(sizeof(UserCard));
    if (!uc) {
        fprintf(stderr, "malloc: error allocating memory for UserCard struct uc\n");
        return NULL;
    }
    uc->service_nickname = strdup(nickname);
    uc->service_website = strdup(website);
    uc->username = strdup(username);
    uc->password = strdup(password);
    return uc;
}

CardDeck* createCardDeck(void)
{
    CardDeck* cd = malloc(sizeof(CardDeck));
    if (cd == NULL) {
        fprintf(stderr, "malloc: error allocating memory for CardDeck struct cd\n");
        return NULL;
    }
    cd->capacity = MAXCARDS;
    cd->count = 0;
    cd->locked = 0;
    cd->cards = calloc(cd->capacity, sizeof(UserCard*));
    if (cd->cards == NULL) {
        fprintf(stderr, "calloc: error allocating %d blocks of UserCard* bytes\n", cd->capacity);
    }
    return cd;
}

void insertUserCard(CardDeck* cd, char* nickname, char* website, char* username, char* password)
{
    UserCard* uc = createUserCard(nickname, website, username, password);
    cd->cards[cd->count] = uc;
    cd->count++;
}

void insertPremadeUserCard(CardDeck* cd, UserCard* uc)
{
    UserCard* new = uc;
    cd->cards[cd->count] = new;
    cd->count++;
}

void freeUserCard(UserCard* uc)
{
    free(uc->service_nickname);
    free(uc->service_website);
    free(uc->username);
    free(uc->password);
    free(uc);
}

void freeCardDeck(CardDeck* cd)
{
    for (int i = 0; i < cd->capacity; i++) {
        UserCard* cur = cd->cards[i];
        if (cur != NULL) {
            freeUserCard(cd->cards[i]);
        }
    }
    free(cd->cards);
    free(cd);
}

void dumpCardDeck(CardDeck* cd)
{
    for (int i = 0; i < cd->count; i++) {
        UserCard* cur = cd->cards[i];
        printf("%s:\n  website: (%s)\n  username: %s\n  password: %s\n", cur->service_nickname, cur->service_website, cur->username, cur->password);
        // printf("%s (%s): %s -> %s\n", cur->service_nickname, cur->service_website, cur->username, cur->password);
    }
}

void dumpDeckInfo(CardDeck* cd)
{
    printf("Locked: %d\nCount: %d\nCapacity: %d\n", cd->locked, cd->count, cd->capacity);
    printf("Services: \n");
    for (int i = 0; i < cd->count; i++) {
        UserCard* cur = cd->cards[i];
        printf("%s ", cur->service_nickname);
    }
}

int saveDeckToFile(CardDeck* cd, char* filename)
{
    FILE* f = fopen(filename, "w");
    if (!f) {
        fprintf(stderr, "fopen: failed to open file %s\n", filename);
        return -1;
    }
    fprintf(f, "%d {\n", cd->locked);
    for (int i = 0; i < cd->count; i++) {
        UserCard* cur = cd->cards[i];
        fprintf(f, "\t%d {\n", i);
        fprintf(f, "\t\t%s,\n", cur->service_nickname);
        fprintf(f, "\t\t%s,\n", cur->service_website);
        fprintf(f, "\t\t%s,\n", cur->username);
        if (i == cd->count - 1) {
            fprintf(f, "\t\t%s\n\t}\n", cur->password);
        } else {
            fprintf(f, "\t\t%s\n\t},\n", cur->password);
        }
    }
    fprintf(f, "}\n\n");
    fclose(f);
    return 0;
}

int readDeckFromFile(CardDeck* cd, char* filename)
{
    int filesize = getSizeOfFile(filename);
    if (filesize == -1) {
        return -1;
    }
    int linecount = getLinesInFile(filename);
    if (linecount <= 0) {
        return -1;
    }
    int fd = open(filename, O_RDONLY);
    if (fd == -1) {
        fprintf(stderr, "open: error opening %s\n", filename);
        return -1;
    }
    char* buffer = malloc((filesize * sizeof(char)) + 1);
    if (buffer == NULL) {
        fprintf(stderr, "malloc: error allocating %d bytes to read %s\n", filesize, filename);
        close(fd);
        return -1;
    }
    ssize_t bRead = read(fd, buffer, filesize);
    if (bRead != filesize) {
        fprintf(stderr, "read: filesize mismatch; expected %d, got %zu\n", filesize, bRead);
        free(buffer);
        close(fd);
        return -1;
    }
    buffer[bRead] = '\0';

    int locked = atoi(&buffer[0]);
    cd->locked = locked;
    int i = 0;

    while (i < filesize) {
        while (i < filesize && !isdigit((unsigned char)buffer[i])) i++;
        if (i >= filesize) break;

        while (i < filesize && isdigit((unsigned char)buffer[i])) i++;

        while (i < filesize && isspace((unsigned char)buffer[i])) i++;

        if (i >= filesize || buffer[i] != '{') {
            i++;
            continue;
        }
        i++;
        int j = i;
        while (j < filesize && isspace((unsigned char)buffer[j])) j++;
        if (j < filesize && isdigit((unsigned char)buffer[j])) {
            continue;
        }
        char* fields[4] = {NULL, NULL, NULL, NULL};
        int ok = 1;
        for (int f = 0; f < 3 && ok; ++f) {
            while (i < filesize && isspace((unsigned char)buffer[i])) i++;
            int start = i;
            while (i < filesize && buffer[i] != ',') i++;
            if (i >= filesize) {
                ok = 0;
                break;
            }
            int end = i;
            while (end > start && isspace((unsigned char)buffer[end - 1])) end--;
            fields[f] = portableStrndup(buffer + start, end - start);
            if (!fields[f]) ok = 0;
            i++;
        }
        if (!ok) {
            for (int k = 0; k < 4; ++k) {
                free(fields[k]);
            }
        }
        while (i < filesize && isspace((unsigned char)buffer[i])) i++;
        int pws = i;
        int pwe = i;
        int done = 0;
        while (i < filesize) {
            if (buffer[i] == '}') {
                pwe = i;
                done = 1;
                break;
            }
            i++;
        }
        if (!done) {
            for (int k = 0; k < 3; ++k) {
                free(fields[k]);
            }
        }
        fields[3] = trimDeckFile(buffer + pws, pwe - pws);
        i++;
        while (i < filesize && isspace((unsigned char)buffer[i])) i++;
        if (i < filesize && buffer[i] == ',') i++;
        insertUserCard(cd, fields[0], fields[1], fields[2], fields[3]);
    }

    free(buffer);
    close(fd);
    return 0;
}

UserCard* findPassWithNickname(CardDeck* cd, char* nickname)
{
    for (int i = 0; i < cd->count; i++) {
        UserCard* cur = cd->cards[i];
        if (strcmp(cur->service_nickname, nickname) == 0) {
            return cur;
        }
    }
    return NULL;
}

void lockCardDeck(CardDeck* cd, char* key)
{
    for (int i = 0; i < cd->count; i++) {
        UserCard* cur = cd->cards[i];
        int len = strlen(cur->password);
        for (int j = 0; j < len; j++) {
            cur->password[j] = cur->password[j] ^ key[j];
        }
    }
    cd->locked = 1;
}

void unlockCardDeck(CardDeck* cd, char* key)
{
    for (int i = 0; i < cd->count; i++) {
        UserCard* cur = cd->cards[i];
        int len = strlen(cur->password);
        for (int j = 0; j < len; j++) {
            cur->password[j] = cur->password[j] ^ key[j];
        }
    }
    cd->locked = 0;
}

// int linecount = getLinesInFile(filename);
// if (linecount == -1) {
//     return -1;
// } else if (linecount == 0) {
//     fprintf(stderr, "error: %s is empty\n", filename);
//     return 0;
// }
