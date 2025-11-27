#ifndef ITALIAN_REPOSITORY_H
#define ITALIAN_REPOSITORY_H

#include <libpq-fe.h>
#include <stdbool.h>

typedef struct {
    char id[37];
    char greet[46];
    bool kind;
    int length;
} ItalianEntity;

int create(const ItalianEntity *rec);
ItalianEntity *read(const char *id);
void update(int id, ItalianEntity entity);
int delete(const char *id);

#endif
