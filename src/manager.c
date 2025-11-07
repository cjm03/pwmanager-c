#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include "central.h"

/* 
 * Program starts by initializing a "Deck" responsible for storing "Cards" containing the data i'm trying to manage. 
 * Next the user must enter a master key which is used to encrypt/decrypt the passwords stored in the cards.
 * A while loop then continuously waits for the user to input an integer representing the action they would like to perform.
 *      1. Add new entry
 *          - This gets a nickname, website, and username from the user. Then the user is asked if they would like to generate
 *            a password or input their own password. The data is then inserted into the deck as a new card.
 *      2. Find a password
 *          - This queries the deck and returns a card assuming the nickname requested exists within the deck.
 *      3. Dump the entries
 *          - Prints all cards in the deck and all the data they contain
 *      4. Lock the deck
 *          - Ensures deck is unlocked, prompts for master key, then encrypts each password in the deck.
 *      5. Unlock the deck
 *          - Same as lock but ensures deck is locked, and decrypts rather than encrypt.
 *      6. Generate and print a new password
 *          - Prompts user for desired length and whether the type should be Simple or Dashed. Prints password to stdout.
 *      7. Save the deck to a file
 *          - Attempts to open the provided filename, and if successful, overwrites the file with the current deck.
 *      8. Load a deck from a file
 *          - Attempts to open the file, then parses each card entry and calls the card insertion function (insertUserCard)
 *      9. Print general info about the current deck
 *          - Prints current count, max capacity, locked state, and all the service nicknames.
 *      0. Quit
 * Locking verifies that the Deck isn't already locked, and requires the user to enter the master key before XORing each character 
 * of each password with the corresponding character of the master key.
 * This is not future proof, and 100% relies on each Card having a password that is shorter than the length of the master key. Unlocking operates in
 * the exact same way.
*/

int main(void)
{
    srand(time(NULL));
    CardDeck* cd = createCardDeck();
    int choice;
    char* key = malloc(64 * sizeof(char));

    printf("\033[1;32m   _____ _____ _____     _____ _ _ _ \033[0m\n");
    printf("\033[1;32m  |   __|   __|   | |___|  _  | | | |\033[0m\n");
    printf("\033[1;32m  |  |  |   __| | | |___|   __| | | |\033[0m\n");
    printf("\033[1;32m  |_____|_____|_|___|   |__|  |_____|\033[0m\n\n");

    printf("No deck loaded. Initialize the key and begin adding entries or load a deck\n");
    printf("Initialize the master key (48-63 chars): ");
    scanf("%s", key);

    while (1) {
        // printf("\n--- PWM ---\n");
        printf("\nOptions:\n");
        printf("    1. Add new entry\n");
        printf("    2. Find a password\n");
        printf("    3. Dump the entries\n");
        printf("    4. Lock the deck\n");
        printf("    5. Unlock the deck\n");
        printf("    6. Generate and print a new password\n");
        printf("    7. Save the deck to a file\n");
        printf("    8. Load a deck from a file\n");
        printf("    9. Print general info about the deck\n");
        printf("    0. Quit\n\n");
        printf("> ");
        scanf("%d", &choice);

////////////////////
// 1: Add a New Entry
////////////////////
        if (choice == 1) {

            UserCard* card = createEmptyUserCard();
            int gen = 0;
            printf("Nickname: ");
            scanf("%s", card->service_nickname);
            printf("Website: ");
            scanf(" %s", card->service_website);
            printf("Username: ");
            scanf(" %s", card->username);
            printf("Generate a password? (0) or provide your own (1): ");
            scanf(" %d", &gen);
            if (gen == 0) {
                int len = 16;
                int type = 0;
                printf("Enter desired password length: ");
                scanf("%d", &len);
                printf("Simple (1) or Dashed (2)?: ");
                scanf("%d", &type);
                if (type == 1) card->password = genSimplePassword(len);
                else card->password = genDashedPassword(len);
                insertUserCard(cd, card->service_nickname, card->service_website, card->username, card->password);
                printf("%s:\n  website: %s\n  username: %s\n  password: %s\n",
                       card->service_nickname, card->service_website, card->username, card->password);
            } else {
                printf("Enter password: ");
                scanf(" %s", card->password);
                insertUserCard(cd, card->service_nickname, card->service_website, card->username, card->password);
                printf("%s:\n  website: %s\n  username: %s\n  password: %s\n",
                       card->service_nickname, card->service_website, card->username, card->password);
            }
            freeUserCard(card);

////////////////////
// 2: Find a Specific Entry
////////////////////
        } else if (choice == 2) {

            char* n = malloc(16 * sizeof(char));                                // ALLOC: n
            printf("Enter service nickname: ");
            scanf("%s", n);
            UserCard* uc = findPassWithNickname(cd, n);
            if (uc) {
                printf("%s:\n  u: %s\n  p: %s\n",
                       n, uc->username, uc->password);
            } else {
                printf("Could not find an entry for %s\n", n);
            }
            free(n);                                                            // FREE: n

////////////////////
// 3: Dump Table Entries
////////////////////
        } else if (choice == 3) {

            dumpCardDeck(cd);

////////////////////
// 4: Lock Deck
////////////////////
        } else if (choice == 4) {

            if (cd->locked == 1) {
                printf("Shit already locked...?\n");
            } else {
                char* attempt = malloc(64 * sizeof(char));          // ALLOC: attempt
                printf("Enter master key: ");
                scanf("%s", attempt);
                if (strcmp(key, attempt) == 0) {
                    lockCardDeck(cd, attempt);
                    printf("Deck locked\n");
                } else {
                    printf("WRONG!!!\n");
                }
                free(attempt);                                      // FREE: attempt
            }

////////////////////
// 5: Unlock Deck
////////////////////
        } else if (choice == 5) {

            if (cd->locked == 0) {
                printf("Shit aint even locked...?\n");
            } else {
                char* attempt = malloc(64 * sizeof(char));          // ALLOC: attempt
                printf("Enter master key: ");
                scanf("%s", attempt);
                if (strcmp(key, attempt) == 0) {
                    unlockCardDeck(cd, attempt);
                    printf("Deck unlocked\n");
                } else {
                    printf("ERROR: master key incorrect\n");
                }
                free(attempt);                                      // FREE: attempt
            }

////////////////////
// 6: Output Generated Password
////////////////////
        } else if (choice == 6) {

            int desiredLength = 16;
            int desiredType = 0;
            char* pwd = malloc(64 * sizeof(char));                          // ALLOC: pwd
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
            free(pwd);                                                      // FREE: pwd

////////////////////
// 7: Save Deck To File
////////////////////
        } else if (choice == 7) {

            char* filename = malloc(32 * sizeof(char));         // ALLOC: filename
            printf("Enter name of file to save to: ");
            scanf("%s", filename);
            int ret = saveDeckToFile(cd, filename);
            if (ret != 0) {
                fprintf(stderr, "error saving to file");
                free(filename);
                break;
            }
            free(filename);                                     // FREE: filename

////////////////////
// 8: Load Deck From File
////////////////////
        } else if (choice == 8) {
            
            char* filename = malloc(32 * sizeof(char));         // ALLOC: filename
            printf("Enter name of file to read from: ");
            scanf("%s", filename);
            int ret = readDeckFromFile(cd, filename);
            if (ret == -1) {
                fprintf(stderr, "error reading from file");
                free(filename);
                break;
            }
            if (cd->locked == 1) {
                printf("loaded deck from %s\n", filename);
                printf("WARNING: DECK IS LOCKED & ENCRYPTED\n");
            } else {
                printf("loaded deck from %s\n", filename);
            }
            free(filename);                                     // FREE: filename

        } else if (choice == 9) {

            dumpDeckInfo(cd);

////////////////////
// 0: QUIT
////////////////////
        } else if (choice == 0) {

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
