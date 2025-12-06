#ifndef ENGLISH_REPOSITORY_H
#define ENGLISH_REPOSITORY_H

#include <libpq-fe.h>
#include <stdbool.h>

typedef struct {
    char id[37];
    char greet[46];
    bool kind;
    int length;
    char *object_json;
    int *listArray;
    int arraySize;
} EnglishEntity;

int englishCreate(const EnglishEntity *entityToSave);
EnglishEntity *englishRead(const char *id);
EnglishEntity **englishReadAll();
int englishUpdate(char *id, EnglishEntity entityToUpdate);
int englishDelete(const char *id);

#endif
