#define _DEFAULT_SOURCE
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <stdint.h>
#include <crypt.h>
#include "central.h"


int main(int argc, char** argv)
{
    srand(time(NULL));
    int interactive = 0;
    int desiredLength;
    int hash;
    if (argc == 1) {
        interactive = 1;
    } else if (argc < 3) {
        fprintf(stderr, "usage: %s -n <LENGTH> [-d:-s] [-h]\n\t-d: produce dashed password\n\t-s: simple, non-dashed password\n    -h: produce the SHA3-256 hash as well\n", argv[0]);
        exit(1);
    } else {

        desiredLength = atoi(argv[2]);
        if (argc >= 4) {
            if (argc == 5) hash = 1;
            char* type = argv[3];
            if (strncmp(type, "-d", 2) == 0) {
                printf("\033[1;32mGenerating DASHED %d-character password...\n\033[0m", desiredLength);
                char* pwd = genDashedPassword(desiredLength);
                printf("\n    \033[1;35mResult: \033[1;91m%s\n\033[0m\n", pwd);
                if (hash == 1) {
                    printf("      \033[1;35mHash: \033[1;91m");
                    int sha = sha256(pwd);
                }
                free(pwd);
                return 0;
            } else if (strncmp(type, "-s", 2) == 0) {
                printf("\033[1;32mGenerating SIMPLE %d-character password...\n\033[0m", desiredLength);
                char* pwd = genSimplePassword(desiredLength);
                printf("\n    \033[1;35mResult: \033[1;91m%s\n\033[0m\n", pwd);
                if (hash == 1) {
                    printf("      \033[1;35mHash: \033[1;91m");
                    int sha = sha256(pwd);
                }
                free(pwd);
                return 0;
            }
        } else {
            printf("\033[1;32mGenerating SIMPLE %d-character password...\n\033[0m", desiredLength);
            char* pwd = genSimplePassword(desiredLength);
            printf("\n    \033[1;35mResult: \033[1;91m%s\n\033[0m\n", pwd);
            free(pwd);
            return 0;
        }
    }

    if (interactive == 1) {
        printf("interactive = 1\n");
    } else {
        exit(EXIT_FAILURE);
    }
    printf("\033[1;32m   _____ _____ _____     _____ _ _ _ \033[0m\n");
    printf("\033[1;32m  |   __|   __|   | |___|  _  | | | |\033[0m\n");
    printf("\033[1;32m  |  |  |   __| | | |___|   __| | | |\033[0m\n");
    printf("\033[1;32m  |_____|_____|_|___|   |__|  |_____|\033[0m\n\n");

    int choice;
    char type = 's';
    char* pwd = genSimplePassword(8);
    while (1) {

        printf("\033[1;32m--- Interactive Mode ---\n\033[0m");
        printf("\033[1;32m    1. Generate a Password\n\033[0m");
        printf("\033[1;32m    2. Hash previous password\n\033[0m");
        printf("\033[1;32m    3. EXIT\n\n\033[0m");
        printf("\033[1;32m> \033[0m");
        scanf("%d", &choice);

        if (choice == 1) {
            free(pwd);
            printf("Length: ");
            scanf("%d", &desiredLength);
            // fflush(stdout);
            printf("(s)imple or (d)ashed: ");
            scanf(" %c", &type);

            if (type == 's') {
                printf("\033[1;32m\nGenerating SIMPLE %d-character password...\n\033[0m", desiredLength);
                pwd = genSimplePassword(desiredLength);
                printf("\n    \033[1;35mResult: \033[1;91m%s\n\033[0m\n", pwd);
            } else if (type == 'd') {
                printf("\033[1;32m\nGenerating DASHED %d-character password...\n\033[0m", desiredLength);
                pwd = genDashedPassword(desiredLength);
                printf("\n    \033[1;35mResult: \033[1;91m%s\n\033[0m\n", pwd);
            } else {
                printf("unsupported type\n");
            }
        } else if (choice == 2) {
            printf("\n\033[1;35mResult: \033[1;91m\n");
            int hash = sha256(pwd);
            printf("\n\033[0m\n%d\n", hash);
        } else if (choice == 3) {
            printf("Goodbye\n");
            free(pwd);
            return 0;
        }
    }
    return 0;
}




