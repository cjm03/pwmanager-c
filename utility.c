#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "central.h"

int getSizeOfFile(char* filename)
{
    FILE* f = fopen(filename, "rb");
    if (f == NULL) {
        fprintf(stderr, "getSizeOfFile: error opening file %s\n", filename);
        return -1;
    }
    fseek(f, 0L, SEEK_END);
    int size = ftell(f);
    fclose(f);
    return size;
}

int getLinesInFile(char* filename)
{
    int ch;
    int lines = 1;
    FILE* f = fopen(filename, "rb");
    if (f == NULL) {
        fprintf(stderr, "getLinesInFile: error opening file %s\n", filename);
        return -1;
    }
    if (fgetc(f) != EOF) {
        rewind(f);
        lines = 1;
    } else {
        fclose(f);
        return 0;
    }
    while ((ch = fgetc(f)) != EOF) {
        if (ch == '\n') {
            lines++;
        }
    }
    fclose(f);
    return lines;
}

char* portableStrndup(char* buffer, int n)
{
    char* p = malloc(n + 1);
    if (!p) return NULL;
    memcpy(p, buffer, n);
    p[n] = '\0';
    return p;
}

char* trimDeckFile(char* buffer, int len)
{
    int start = 0;
    while (start < len && isspace((unsigned char)buffer[start])) {
        start++;
    }
    int end = len;
    while (end > start && isspace((unsigned char)buffer[end - 1])) {
        end--;
    }
    return portableStrndup(buffer + start, end - start);
}
