#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include "pwgen.h"
#include "storage.h"

/* 
 * Program starts by initializing the "Deck" responsible for storing the "Cards" containing the data i'm trying to manage. I load the example data
 * from the function under main, which generates 16 passwords, creates 16 Cards with the passwords, inserts them into the Deck, and then frees the
 * temporary pointers. Then, I declare that the Deck is unlocked and allocate 64 characters to the key which the user must populate with a strong
 * master key in order to lock/unlock the Deck. Next a while loop keeps the program running and provides a few options. Adding a new entry works,
 * but I don't like the way that it feels so I will work on that in the near future. Find a password simply queries the Deck, asking if any card 
 * has the same nickname that the user is searching for. Dump entries = dump entries to stdout. Locking verifies that the Deck isn't already locked,
 * and requires the user to enter the master key before XORing each character of each password with the corresponding character of the master key.
 * This is not future proof, and 100% relies on each Card having a password that is shorter than the length of the master key. Unlocking operates in
 * the exact same way.
*/

void loadTest(CardDeck* cd);

int main(void)
{
    srand(time(NULL));
    CardDeck* cd = createCardDeck();
    loadTest(cd);
    int choice;
    int locked = 0;
    char* key = malloc(64 * sizeof(char));

    printf("\033[1;32m   _____ _____ _____     _____ _ _ _ \033[0m\n");
    printf("\033[1;32m  |   __|   __|   | |___|  _  | | | |\033[0m\n");
    printf("\033[1;32m  |  |  |   __| | | |___|   __| | | |\033[0m\n");
    printf("\033[1;32m  |_____|_____|_|___|   |__|  |_____|\033[0m\n\n");

    printf("Initialize the master key (48-63 chars): ");
    scanf("%s", key);

    while (1) {
        printf("\n--- PWM ---\n");
        printf("    1. Add new entry\n");
        printf("    2. Find a password\n");
        printf("    3. Dump the entries\n");
        printf("    4. Lock the deck\n");
        printf("    5. Unlock the deck\n");
        printf("    6. Generate and print a new password\n");
        // TODO: printf("    7. Save the deck to a file\n");
        printf("    7. Quit\n\n");
        printf("> ");
        scanf("%d", &choice);

/* 1 */ if (choice == 1) {
            char* n = malloc(16 * sizeof(char));    // This all feels highly unoptimized
            char* w = malloc(32 * sizeof(char));
            char* u = malloc(16 * sizeof(char));
            char* p = malloc(48 * sizeof(char));
            int gen = 0;
            printf("Nickname: ");
            scanf("%s", n);
            printf("Website: ");
            scanf(" %s", w);
            printf("Username: ");
            scanf(" %s", u);
            printf("Generate a password? (0) or provide your own (1): ");
            scanf(" %d", &gen);
            if (gen == 0) {
                p = genDashedPassword(20);
                insertUserCard(cd, n, w, u, p);
                printf("%s:\n  website: %s\n  username: %s\n  password: %s\n", n, w, u, p);
                free(n);
                free(w);
                free(u);
                free(p);
            } else {
                printf("Enter password: ");
                scanf(" %s", p);
                insertUserCard(cd, n, w, u, p);
                printf("%s:\n  website: %s\n  username: %s\n  password: %s\n", n, w, u, p);
                free(n);
                free(w);
                free(u);
                free(p);
            }
/* 2 */ } else if (choice == 2) {

            char* n = malloc(16 * sizeof(char));
            printf("Enter service nickname: ");
            scanf("%s", n);
            UserCard* uc = findPassWithNickname(cd, n);

            if (uc) {
                printf("%s:\n  u: %s\n  p: %s\n", n, uc->username, uc->password);
            } else {
                printf("Could not find an entry for %s\n", n);
            }
            free(n);

/* 3 */ } else if (choice == 3) {

            dumpCardDeck(cd);

/* 4 */ } else if (choice == 4) {

            if (locked == 1) {
                printf("Shit already locked...?\n");
            } else {
                char* attempt = malloc(64 * sizeof(char));
                printf("Enter master key: ");
                scanf("%s", attempt);
                if (strcmp(key, attempt) == 0) {
                    lockCardDeck(cd, attempt);
                    printf("Deck locked\n");
                    free(attempt);
                    locked = 1;
                } else {
                    free(attempt);
                    printf("WRONG!!!\n");
                }
            }
/* 5 */ } else if (choice == 5) {

            if (locked == 0) {
                printf("Shit aint even locked...?\n");
            } else {
                char* attempt = malloc(64 * sizeof(char));
                printf("Enter master key: ");
                scanf("%s", attempt);
                if (strcmp(key, attempt) == 0) {
                    lockCardDeck(cd, attempt);
                    printf("Deck unlocked\n");
                    free(attempt);
                    locked = 0;
                } else {
                    free(attempt);
                    printf("WRONG!!!\n");
                }
            }
/* 6 */ } else if (choice == 6) {

            int desiredLength = 16;
            int desiredType = 0;
            char* pwd = malloc(64 * sizeof(char));
            printf("Enter desired length (>=16 recommended): ");
            scanf("%d", &desiredLength);
            printf("Enter 1 for simple or 2 for dashed: ");
            scanf("%d", &desiredType);
            if (desiredLength <= 0 || desiredLength >= 64) desiredLength = 16;
            if (desiredType == 1) {
                printf("\033[1;32m\nGenerating SIMPLE %d-character password...\n\033[0m", desiredLength);
                pwd = genSimplePassword(desiredLength);
                printf("\n    \033[1;35mResult: \033[1;91m%s\n\033[0m\n", pwd);
            } else if (desiredType == 2) {
                printf("\033[1;32m\nGenerating DASHED %d-character password...\n\033[0m", desiredLength);
                pwd = genDashedPassword(desiredLength);
                printf("\n    \033[1;35mResult: \033[1;91m%s\n\033[0m\n", pwd);
            } else {
                printf("\033[1;32m\nGenerating SIMPLE %d-character password...\n\033[0m", desiredLength);
                pwd = genSimplePassword(desiredLength);
                printf("\n    \033[1;35mResult: \033[1;91m%s\n\033[0m\n", pwd);
            }
            free(pwd);

/* 7 */ } else if (choice == 7) {

            printf("BYE!\n");
            break;

        } else {
            printf("unsupported, I suggest reading the options\n");
            break;
        }
    }
    free(key);
    freeCardDeck(cd);
    return 0;
}


void loadTest(CardDeck* cd)
{
    int len = 32;
    char* apwd = genSimplePassword(len);
    char* bpwd = genDashedPassword(len);
    char* cpwd = genSimplePassword(len);
    char* dpwd = genDashedPassword(len);
    char* epwd = genSimplePassword(len);
    char* fpwd = genDashedPassword(len);
    char* gpwd = genSimplePassword(len);
    char* hpwd = genDashedPassword(len);
    char* ipwd = genSimplePassword(len);
    char* jpwd = genDashedPassword(len);
    char* kpwd = genSimplePassword(len);
    char* lpwd = genDashedPassword(len);
    char* mpwd = genSimplePassword(len);
    char* npwd = genDashedPassword(len);
    char* opwd = genSimplePassword(len);
    char* ppwd = genDashedPassword(len);
    insertUserCard(cd, "apple",     "www.apple.com",    "alex123",       apwd);
    insertUserCard(cd, "boeing",    "www.boeing.com",   "bethanyx0",     bpwd);
    insertUserCard(cd, "comcast",   "www.comcast.com",  "crab",          cpwd);
    insertUserCard(cd, "dingledog", "www.ddog.com",     "d00by",         dpwd);
    insertUserCard(cd, "elepo",     "www.elepo.com",    "xelden1",       epwd);
    insertUserCard(cd, "frockling", "www.frockling.com","felix2",        fpwd);
    insertUserCard(cd, "github",    "www.github.com",   "go0b13",        gpwd);
    insertUserCard(cd, "hackle",    "www.htb.com",      "henry1x",       hpwd);
    insertUserCard(cd, "iosevka",   "www.iosevka.com",  "ingr1dx",       ipwd);
    insertUserCard(cd, "jackal",    "www.jackal.com",   "jack455",       jpwd);
    insertUserCard(cd, "kernel",    "www.kernel.com",   "k3rn3l3nj0y3r", kpwd);
    insertUserCard(cd, "lalo",      "www.lalo.com",     "linguine",      lpwd);
    insertUserCard(cd, "minecraft", "www.minecraft.com","mexican4",      mpwd);
    insertUserCard(cd, "netflix",   "www.netflix.com",  "na1v3",         npwd);
    insertUserCard(cd, "ollama",    "www.ollama.com",   "os1nt",         opwd);
    insertUserCard(cd, "penis",     "www.penis.com",    "pickel",        ppwd);
    free(apwd);
    free(bpwd);
    free(cpwd);
    free(dpwd);
    free(epwd);
    free(fpwd);
    free(gpwd);
    free(hpwd);
    free(ipwd);
    free(jpwd);
    free(kpwd);
    free(lpwd);
    free(mpwd);
    free(npwd);
    free(opwd);
    free(ppwd);
}
