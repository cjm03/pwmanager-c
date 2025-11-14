#include <strings.h>
#define _DEFAULT_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <ctype.h>
#include <math.h>

#include "central.h"
#include "../external/aes.h"
#include "../external/allhead.h"

static UserCard DELETEDCARD = { NULL, NULL, NULL, NULL, NULL, 0};

void DumpHashCardDeck(CardDeck* cd)
{
    for (int i = 0; i < cd->capacity; i++) {
        UserCard* cur = cd->cards[i];
        if (cur != NULL) {
            int idx = GetSimpleHash(cur->service_nickname, cd->capacity, 0);
            if (idx == -1) continue;
            printf("[%d]\n  %s:\n    +website: (%s)\n    +username: %s\n    +password: %s\n    +before: %d\n", idx, cur->service_nickname, cur->service_website, cur->username, cur->password, cur->before);
            while (cur->next != NULL) {
                printf("  %s:\n    +website: (%s)\n    +username: %s\n    +password: %s\n    +before: %d\n", cur->service_nickname, cur->service_website, cur->username, cur->password, cur->before);
                cur = cur->next;
            }
        }
    }
}

void DumpHashCardDeckInfo(CardDeck* cd)
{
    printf("Locked: %d\nCount: %d\nCapacity: %d\n", cd->locked, cd->count, cd->capacity);
}

int saveDeckToFile(CardDeck* cd, char* filename)
{
    FILE* f = fopen(filename, "w");
    if (!f) {
        fprintf(stderr, "fopen: failed to open file %s\n", filename);
        return -1;
    }
    fprintf(f, "%d {\n", cd->locked);
    int cnt = 0;
    for (int i = 0; i < cd->capacity; i++) {
        UserCard* cur = cd->cards[i];
        if (cur != NULL) {
            cnt++;
            fprintf(f, "\t%d {\n", i);
            fprintf(f, "\t\t%s,\n", cur->service_nickname);
            fprintf(f, "\t\t%s,\n", cur->service_website);
            fprintf(f, "\t\t%s,\n", cur->username);
            if (cnt == cd->count) {
                fprintf(f, "\t\t%s\n\t}\n", cur->password);
            } else {
                fprintf(f, "\t\t%s\n\t},\n", cur->password);
            }
        }
    }
    fprintf(f, "}\n\n");
    fclose(f);
    return 0;
}

int readDeckFromFile(M_Arena* arena, CardDeck* cd, char* filename)
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
    char* buffer = ArenaAlloc(arena, (filesize * sizeof(char)) + 1);
    if (buffer == NULL) {
        fprintf(stderr, "malloc: error allocating %d bytes to read %s\n", filesize, filename);
        close(fd);
        return -1;
    }
    ssize_t bRead = read(fd, buffer, filesize);
    if (bRead != filesize) {
        fprintf(stderr, "read: filesize mismatch; expected %d, got %zu\n", filesize, bRead);
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
            fields[f] = MStrndup(arena, buffer + start, end - start);
            if (!fields[f]) ok = 0;
            i++;
        }
        if (!ok) {
            for (int k = 0; k < 4; ++k) {
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
        fields[3] = trimDeckFile(arena, buffer + pws, pwe - pws);
        i++;
        while (i < filesize && isspace((unsigned char)buffer[i])) i++;
        if (i < filesize && buffer[i] == ',') i++;
        InsertHashUserCard(arena, cd, fields[0], fields[1], fields[2], fields[3]);
    }

    close(fd);
    return 0;
}

UserCard* FindHashPassWithNickname(CardDeck* cd, char* nickname)
{
    int index = GetSimpleHash(nickname, cd->capacity, 0);
    UserCard* cur = cd->cards[index];
    if (cur->service_nickname == NULL) return NULL;
    while (cur != NULL) {
        printf("cur->nick: %s\n", cur->service_nickname);
        if (strcmp(cur->service_nickname, nickname) == 0) {
            return cur;
        }
        cur = cur->next;
    }
    return NULL;
    // if (cur == NULL) return NULL;
    // if (strcmp(cur->service_nickname, nickname) == 0) {
    //     return cur;
    // } else if (cur->next != NULL) {
    //     while (cur->next != NULL) {
    //         cur = cur->next;
    //         if (strcmp(cur->service_nickname, nickname) == 0) {
    //             return cur;
    //         }
    //     }
    // }
    // return NULL;
}

int DeleteHashCard(CardDeck* cd, char* nickname)
{
    // UserCard* uc = FindHashPassWithNickname(cd, nickname);
    // if (uc == NULL) {
    //     printf("[%s] wasn't found\n", nickname);
    //     return 0;
    // }
    int index = GetSimpleHash(nickname, cd->capacity, 0);
    UserCard* cur = cd->cards[index];
    while (cur != NULL) {
        if (cur != &DELETEDCARD) {
            if (strcmp(cur->service_nickname, nickname) == 0) {
                memset(cur->service_nickname, 0, MAX_NICKNAME_LEN);
                memset(cur->service_website, 0, MAX_WEBSITE_LEN);
                memset(cur->username, 0, MAX_USERNAME_LEN);
                memset(cur->password, 0, MAX_PASSWORD_LEN);
                cd->cards[index] = &DELETEDCARD;
                // uc->service_nickname = NULL;
                // uc->service_website = NULL;
                // uc->username = NULL;
                // uc->password = NULL;
                // uc->next = NULL;
                cd->count--;
                return 1;
            }
        }
        if (strcmp(cur->next->service_nickname, nickname) == 0) {
            UserCard* t = cur->next;
            if (t->next == NULL) {
                cur->next = t->next;
            } else {
                cur->next = NULL;
            }
            t = &DELETEDCARD;
        }
        cur = cur->next;
        // } else {
        //     while (cur->next != NULL) {
        //         if (strcmp(uc->service_nickname, cur->next->service_nickname) == 0) {
        //             cur->next = uc->next;
        //             uc->service_nickname = NULL;
        //             uc->service_website = NULL;
        //             uc->username = NULL;
        //             uc->password = NULL;
        //             uc->next = NULL;
        //             cd->count--;
        //             return 1;
        //         }
        //         cur = cur->next;
        //     }
        // }
    }
    return 0;
}

void AESLockDeck(CardDeck* cd, char* key)
{
    uint8_t nkey[strlen(key)];
    StrToHex(key, nkey, strlen(key));
    struct AES_ctx ctx;
    AES_init_ctx(&ctx, nkey);
    for (int i = 0; i < cd->capacity; i++) {
        UserCard* cur = cd->cards[i];
        if (cur != NULL) {
            AES_ECB_encrypt(&ctx, (unsigned char*)cur->password);
            while (cur->next != NULL) {
                cur = cur->next;
                AES_ECB_encrypt(&ctx, (unsigned char*)cur->password);
            }
        }
    }
    cd->locked = 1;
}

void AESUnlockDeck(CardDeck* cd, char* key)
{
    uint8_t nkey[strlen(key)];
    StrToHex(key, nkey, strlen(key));
    struct AES_ctx ctx;
    AES_init_ctx(&ctx, nkey);
    for (int i = 0; i < cd->capacity; i++) {
        UserCard* cur = cd->cards[i];
        if (cur != NULL) {
            AES_ECB_decrypt(&ctx, (unsigned char*)cur->password);
            while (cur->next != NULL) {
                cur = cur->next;
                AES_ECB_decrypt(&ctx, (unsigned char*)cur->password);
            }
        }
    }
    cd->locked = 0;
}

char* MStrndup(M_Arena* arena, char* data, size_t n) {
    char* p = ArenaAlloc(arena, n + 1);
    memcpy(p, data, n);
    p[n] = '\0';
    return p;
}

// Hashing

int SimpleHash(const char* identifier, const int prime, const int mod)
{
    if (identifier == NULL) return -1;
    long hash = 0;
    const int len = strlen(identifier);
    for (int i = 0; i < len; i++) {
        hash += (long)pow(prime, len - (i + 1)) * identifier[i];
        hash = hash % mod;
    }
    return (int)hash;
}

int GetSimpleHash(const char* identifier, const int mod, const int attempt)
{
    if (identifier == NULL) return -1;
    const int a = SimpleHash(identifier, HASH_PRIME1, mod);
    const int b = SimpleHash(identifier, HASH_PRIME2, mod);
    return (a + (attempt * (b + 1))) % mod;
}


CardDeck* CreateHashCardDeck(M_Arena* arena)
{
    CardDeck* cd = ArenaAlloc(arena, sizeof(CardDeck));
    cd->capacity = MAXCARDS;
    cd->count = 0;
    cd->locked = 0;
    cd->cards = ArenaAlloc(arena, cd->capacity * sizeof(UserCard*));
    return cd;
}

UserCard* CreateHashUserCard(M_Arena* arena, char* nickname, char* website, char* username, char* password)
{
    UserCard* uc = ArenaAlloc(arena, sizeof(UserCard));
    uc->service_nickname = MStrndup(arena, nickname, strlen(nickname));
    uc->service_website = MStrndup(arena, website, strlen(website));
    uc->username = MStrndup(arena, username, strlen(username));
    uc->password = MStrndup(arena, password, strlen(password));
    return uc;
}

UserCard* CreateHashEmptyUserCard(M_Arena* arena)
{
    UserCard* uc = ArenaAlloc(arena, sizeof(UserCard));
    uc->service_nickname = ArenaAlloc(arena, MAX_NICKNAME_LEN);
    uc->service_website = ArenaAlloc(arena, MAX_WEBSITE_LEN);
    uc->username = ArenaAlloc(arena, MAX_USERNAME_LEN);
    uc->password = ArenaAlloc(arena, MAX_PASSWORD_LEN);
    return uc;
}

void InsertHashUserCard(M_Arena* arena, CardDeck* cd, char* nickname, char* website, char* username, char* password)
{
    UserCard* uc = CreateHashUserCard(arena, nickname, website, username, password);
    int index = GetSimpleHash(uc->service_nickname, cd->capacity, 0);
    UserCard* cur = cd->cards[index];
    int cnt = 1;
    while (cur != NULL) {
        if (cur != &DELETEDCARD) {
            if (cur->next == NULL) {
                cur->next = ArenaAlloc(arena, sizeof(UserCard*));
                cur->next = uc;
                uc->before = cnt;
                cd->count++;
                return;
            }
        }
        cur = cur->next;
        cnt++;
    }
    cd->cards[index] = uc;
    uc->next = NULL;
    cd->count++;
}
