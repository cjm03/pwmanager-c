#ifndef PWGEN_H
#define PWGEN_H

int genSalt(unsigned char* buffer, int bytes);
char genCharacter(void);
char* genDashedPassword(int len);
char* genSimplePassword(int len);
char genCharacterForDashed(void);
int sha256(char* pwd);


#endif // PWGEN_H
