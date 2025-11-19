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
 * Locking verifies that the Deck isn't already locked, and requires the user to enter the master key. Each card's password element is then encrypted
 * using AES with the master key.
 * Unlocking operates in the exact same way. The program does not remember your master key, so don't lose it!
*/

int main(void)
{
    srand(time(NULL));
    M_Arena STRS;
    ArenaInitSized(&STRS, Kilobytes(4));
    CardDeck* cd = CreateHashCardDeck();
    char* key = malloc(64 * sizeof(char));

    printf("\033[1;32m   _____ _____ _____     _____ _ _ _ \033[0m\n");
    printf("\033[1;32m  |   __|   __|   | |___|  _  | | | |\033[0m\n");
    printf("\033[1;32m  |  |  |   __| | | |___|   __| | | |\033[0m\n");
    printf("\033[1;32m  |_____|_____|_|___|   |__|  |_____|\033[0m\n\n");

    printf("No deck loaded. Initialize the key and begin adding entries or load a deck\n");
    printf("Initialize the master key (48-63 chars): ");
    scanf("%s", key);

    printf("\n===Options===\n");
    printf("new - Add new entry\n");
    printf("find <nickname> - Find a password\n");
    printf("dump - Dump the entries\n");
    printf("lock - Lock the deck\n");
    printf("unlock - Unlock the deck\n");
    printf("delete <nickname> - Delete an entry\n");
    printf("save <filename> - Save the deck to a file\n");
    printf("load <filename> - Load a deck from a file\n");
    printf("unload - Unload a deck from Memory\n");
    printf("help - Print this menu\n");
    printf("exit - Quit\n\n");

    while (1) {
        printf("> ");
        char* command = GETSTRING();

    ////////////////////
    // 1: Add a New Entry
    ////////////////////
        if (strncmp(command, "new", 3) == 0) {

            uAddNewUserEntry(cd);

    ////////////////////
    // 2: Find an Entry
    ////////////////////
        } else if (strncmp(command, "find", 4) == 0) {

            if (strtok(command, " ") != NULL) {
                char* nick = strtok(NULL, " ");
                if (nick != NULL) {
                    UserCard* uc = FindHashPassWithNickname(cd, nick);
                        if (uc) {
                            printf("\033[1;32m\n%s\n\033[0m    \033[1;35mu: \033[1;91m%s\n\033[0m    \033[1;35mp: \033[1;91m%s\n\033[0m\n",
                                   nick, uc->username, uc->password);
                        } else {
                            printf("Could not find an entry for %s\n", nick);
                        }
                }
            }

            // char* n = ArenaAlloc(&STRS, MAX_NICKNAME_LEN);                                // ALLOC: n
            // printf("Enter service nickname: ");
            // scanf("%s", n);
            // UserCard* uc = NULL;
            // uc = FindHashPassWithNickname(cd, n);
            // if (uc) {
            //     printf("\033[1;32m\n%s\n\033[0m    \033[1;35mu: \033[1;91m%s\n\033[0m    \033[1;35mp: \033[1;91m%s\n\033[0m\n",
            //            n, uc->username, uc->password);
            // } else {
            //     printf("Could not find an entry for %s\n", n);
            // }

    ////////////////////
    // 3: Dump Table Entries
    ////////////////////
        } else if (strncmp(command, "dump", 4) == 0) {

            DumpHashCardDeck(cd);

    ////////////////////
    // 4: Lock Deck
    ////////////////////
        } else if (strncmp(command, "lock", 4) == 0) {

            if (cd->locked == 1) {
                printf("Shit already locked...?\n");
            } else {
                char* attempt = ArenaAlloc(&STRS, MAX_PASSWORD_LEN);          // ALLOC: attempt
                printf("Enter master key: ");
                scanf("%s", attempt);
                if (strcmp(key, attempt) == 0) {
                    AESLockDeck(cd, attempt);
                    printf("Deck locked\n");
                } else {
                    printf("WRONG!!!\n");
                }
            }

    ////////////////////
    // 5: Unlock Deck
    ////////////////////
        } else if (strncmp(command, "unlock", 6) == 0) {

            if (cd->locked == 0) {
                printf("Shit aint even locked...?\n");
            } else {
                char* attempt = ArenaAlloc(&STRS, MAX_PASSWORD_LEN);          // ALLOC: attempt
                printf("Enter master key: ");
                scanf("%s", attempt);
                if (strcmp(key, attempt) == 0) {
                    AESUnlockDeck(cd, attempt);
                    printf("Deck unlocked\n");
                } else {
                    printf("ERROR: master key incorrect\n");
                }
            }

    ////////////////////
    // 6: Delete an Entry 
    ////////////////////
        } else if (strncmp(command, "delete", 6) == 0) {

            if (strtok(command, " ") != NULL) {
                char* todelete = strtok(NULL, " ");
                if (todelete != NULL) {
                    int ret = DeleteHashCard(cd, todelete);
                    if (ret == 1) printf("NULLED [%s]\n", todelete);
                }
            } else {
                printf("error: delete requires the nickname to be deleted: delete <nickname>\n");
            }

            // char* todelete = ArenaAlloc(&STRS, MAX_NICKNAME_LEN);
            // printf("Enter nickname of entry to delete: ");
            // scanf("%s", todelete);
            // int ret = DeleteHashCard(cd, todelete);
            // if (ret == 1) printf("NULLED [%s]\n", todelete);

    ////////////////////
    // 7: Save Deck
    ////////////////////
        } else if (strncmp(command, "save", 4) == 0) {

            if (strtok(command, " ") != NULL) {
                char* filename = strtok(NULL, " ");
                if (filename != NULL) {
                    int ret = saveDeckToFile(cd, filename);
                    if (ret != 0) {
                        printf("error saving to file %s\n", filename);
                        break;
                    }
                }
            }

            // char* filename = ArenaAlloc(&STRS, 32);         // ALLOC: filename
            // printf("Enter name of file to save to: ");
            // scanf("%s", filename);
            // int ret = saveDeckToFile(cd, filename);
            // if (ret != 0) {
            //     fprintf(stderr, "error saving to file");
            //     break;
            // }

    ////////////////////
    // 8: Load Deck
    ////////////////////
        } else if (strncmp(command, "load", 4) == 0) {
            
            if (strtok(command, " ") != NULL) {
                char* filename = strtok(NULL, " ");
                if (filename != NULL) {
                    int ret = readDeckFromFile(cd, filename);
                    if (ret == -1) {
                        printf("error reading from file %s\n", filename);
                        break;
                    }
                    if (cd->locked == 1) {
                        printf("loaded deck from %s\n", filename);
                        printf("WARNING: DECK IS LOCKED & ENCRYPTED\n");
                    } else {
                        printf("loaded deck from %s\n", filename);
                    }
                }
            }
            // char* filename = ArenaAlloc(&STRS, 32);         // ALLOC: filename
            // printf("Enter name of file to read from: ");
            // scanf("%s", filename);
            // int ret = readDeckFromFile(cd, filename);
            // if (ret == -1) {
            //     fprintf(stderr, "error reading from file");
            //     break;
            // }
            // if (cd->locked == 1) {
            //     printf("loaded deck from %s\n", filename);
            //     printf("WARNING: DECK IS LOCKED & ENCRYPTED\n");
            // } else {
            //     printf("loaded deck from %s\n", filename);
            // }

    ////////////////////
    // 9: Unload Deck 
    ////////////////////
        } else if (strncmp(command, "unload", 6) == 0) {

            ArenaClear(&STRS);

        } else if (strncmp(command, "help", 4) == 0) {

            printf("\n===Options===\n");
            printf("new - Add new entry\n");
            printf("find <nickname> - Find a password\n");
            printf("dump - Dump the entries\n");
            printf("lock - Lock the deck\n");
            printf("unlock - Unlock the deck\n");
            printf("delete <nickname> - Delete an entry\n");
            printf("save <filename> - Save the deck to a file\n");
            printf("load <filename> - Load a deck from a file\n");
            printf("unload - Unload a deck from Memory\n");
            printf("help - Print this menu\n");
            printf("exit - Quit\n\n");

    ////////////////////
    // 0: QUIT
    ////////////////////
        } else if (strncmp(command, "exit", 4) == 0) {

            printf("BYE!\n");
            free(command);
            break;

        }
        free(command);
        // } else {
        //     printf("unsupported, I suggest reading the options\n");
        //     break;

    }
    FreeHashDeck(cd);
    free(key);
    ArenaFree(&STRS);
    return 0;
}
