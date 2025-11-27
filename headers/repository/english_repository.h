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
    int list_size;
} EnglishEntity;

int create(const EnglishEntity *rec);
EnglishEntity *read(const char *id);
void update(int id, EnglishEntity entity);
int delete(const char *id);

#endif
