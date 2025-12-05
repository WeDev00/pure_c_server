#ifndef ENGLISH_SERVICE_H
#define ENGLISH_SERVICE_H

#include "../../headers/repository/english_repository.h"

int englishServiceCreate(EnglishEntity entity);
EnglishEntity *englishServiceRead(const char *id);
EnglishEntity **englishServiceReadAll();
int englishServiceUpdate(char *id, EnglishEntity entity);
int englishServiceDelete(const char *id);

#endif
