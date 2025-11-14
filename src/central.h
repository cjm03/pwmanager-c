#ifndef CENTRAL_H
#define CENTRAL_H

#include <stdint.h>
#include <stddef.h>

// #include "../external/aes.h"
#include "../external/allhead.h"

// Defines
#define MAXCARDS 256        // storage.h
#define HASH_PRIME1 10837
#define HASH_PRIME2 11683
#define MAX_NICKNAME_LEN 32
#define MAX_WEBSITE_LEN 32
#define MAX_USERNAME_LEN 32
#define MAX_PASSWORD_LEN 64

// Structures
typedef struct UserCard {   // storage.h
    char* service_nickname;
    char* service_website;
    char* username;
    char* password;
    struct UserCard* next;
    int before;
} UserCard;
typedef struct CardDeck {   // storage.h
    int capacity;
    int count;
    int locked;
    UserCard** cards;
} CardDeck;

///////////// 
// pwgen.h //
/////////////

// generate BYTES bytes of salt and write to BUFFER
int genSalt(unsigned char* buffer, int bytes);

// generate a random character in a-zA-Z0-9!@#$_-
char genCharacter(void);

// randomly generate a password of roughly LEN length. LEN will be altered to allow for password of form(s):
// 123456-098765 or abcdef-ghijkl-123456 or !@#$_0-123456-uvwxyz-ZYXWVU-abcdef
char* genDashedPassword(M_Arena* arena, int len);

// randomly generate a password of LEN length.
char* genSimplePassword(M_Arena* arena, int len);

// generate a random character in a-zA-Z0-9!@#$_
char genCharacterForDashed(void);

// print the SHA3-256 hash of PWD to stdout. returns 0 on success 
int sha256(char* pwd);


/////////////// 
// storage.h //
///////////////


// write the contents of CardDeck CD to file FILENAME. returns 0 on success
int saveDeckToFile(CardDeck* cd, char* filename);

// read the contents of file FILENAME, parse, and populate the CardDeck CD with the contents. returns 0 on success
int readDeckFromFile(M_Arena* arena, CardDeck* cd, char* filename);

// encrypt each card->password in CardDeck CD with key KEY using AES
void AESLockDeck(CardDeck* cd, char* key);

// decrypt each card->password in CardDeck CD with key KEY using AES
void AESUnlockDeck(CardDeck* cd, char* key);

char* MStrndup(M_Arena* arena, char* data, size_t n);
int SimpleHash(const char* identifier, const int prime, const int mod);
int GetSimpleHash(const char* identifier, const int mod, const int attempt);
// allocate memory for a CardDeck. return the CardDeck
CardDeck* CreateHashCardDeck(M_Arena* arena);
// allocate memory and populate a UserCard and its elements. return the UserCard
UserCard* CreateHashUserCard(M_Arena* arena, char* nickname, char* website, char* username, char* password);
// allocate memory for a UserCard and its elements but do not initialize. return the UserCard
UserCard* CreateHashEmptyUserCard(M_Arena* arena);
// create a UserCard with the passed data, and insert the card into the CardDeck CD.
void InsertHashUserCard(M_Arena* arena, CardDeck* cd, char* nickname, char* website, char* username, char* password);
// print the contents of CardDeck CD's cards
void DumpHashCardDeck(CardDeck* cd);
// print basic info found directly in CD's CardDeck struct
void DumpHashCardDeckInfo(CardDeck* cd);
UserCard* FindHashPassWithNickname(CardDeck* cd, char* nickname);
int DeleteHashCard(CardDeck* cd, char* nickname);


///////////////
// utility.h //
///////////////

// fseek to the end of file FILENAME, return the position representing the file's byte count
int getSizeOfFile(char* filename);

// return the number of lines in file FILENAME (often returns 1 more than actual line count)
int getLinesInFile(char* filename);

// copy N bytes of BUFFER into a new buffer which is returned.
char* portableStrndup(char* buffer, int n);

// remove N bytes of unnecessary whitespace from BUFFER. return the trimmed buffer via portableStrndup
char* trimDeckFile(M_Arena* arena, char* buffer, int len);

// iterate through each character in IN, cast it to uint8_t, assign it to the matching index in OUT
void StrToHex(char* in, uint8_t* out, size_t length);

void uAddNewUserEntry(M_Arena* arena, CardDeck* deck, UserCard* card);

void uGeneratePassword(M_Arena* arena);

#endif // CENTRAL_H
