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

int italianCreate(const ItalianEntity *rec);
ItalianEntity *italianRead(const char *id);
void italianUpdate(int id, ItalianEntity entity);
int italianDelete(const char *id);

#endif
