#ifndef STORAGE_H
#define STORAGE_H

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

#endif // STORAGE_H
