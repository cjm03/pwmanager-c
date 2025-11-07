#ifndef CENTRAL_H
#define CENTRAL_H

/* 
 * pwgen.h
*/
int genSalt(unsigned char* buffer, int bytes);
char genCharacter(void);
char* genDashedPassword(int len);
char* genSimplePassword(int len);
char genCharacterForDashed(void);
int sha256(char* pwd);

/* 
 * storage.h
*/
#define MAXCARDS 256
typedef struct UserCard {
    char* service_nickname;
    char* service_website;
    char* username;
    char* password;
} UserCard;
typedef struct CardDeck {
    int capacity;
    int count;
    int locked;
    UserCard** cards;
} CardDeck;
UserCard* createEmptyUserCard(void);
UserCard* createUserCard(char* nickname, char* website, char* username, char* password);
CardDeck* createCardDeck(void);
void insertUserCard(CardDeck* cd, char* nickname, char* website, char* username, char* password);
void insertPremadeUserCard(CardDeck* cd, UserCard* uc);
void freeUserCard(UserCard* uc);
void freeCardDeck(CardDeck* cd);
void dumpCardDeck(CardDeck* cd);
void dumpDeckInfo(CardDeck* cd);
int saveDeckToFile(CardDeck* cd, char* filename);
int readDeckFromFile(CardDeck* cd, char* filename);
UserCard* findPassWithNickname(CardDeck* cd, char* nickname);
void lockCardDeck(CardDeck* cd, char* key);
void unlockCardDeck(CardDeck* cd, char* key);

/*
 * utility.h
*/
int getSizeOfFile(char* filename);
int getLinesInFile(char* filename);
char* portableStrndup(char* buffer, int n);
char* trimDeckFile(char* buffer, int len);

#endif // CENTRAL_H
