#ifndef UTILITY_H
#define UTILITY_H

int getSizeOfFile(char* filename);

int getLinesInFile(char* filename);

char* portableStrndup(char* buffer, int n);

char* trimDeckFile(char* buffer, int len);

#endif // UTILITY_H
