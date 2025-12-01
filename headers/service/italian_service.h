#ifndef ITALIAN_SERVICE_H
#define ITALIAN_SERVICE_H

#include "../repository/italian_repository.h"

int italianServiceCreate(ItalianEntity entity);
ItalianEntity *italianServiceRead(const char *id);
ItalianEntity **italianServiceReadAll();
int italianServiceUpdate(char *id, ItalianEntity entity);
int italianServiceDelete(const char *id);

#endif
