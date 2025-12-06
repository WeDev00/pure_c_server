#include "../../headers/service/english_service.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int englishServiceCreate(EnglishEntity entity) {
    int result = englishCreate(&entity);
    if (result == 0)
        printf("[SERVICE] English created successfully\n");

    return result;
}
EnglishEntity *englishServiceRead(const char *id) {
    EnglishEntity *entity = englishRead(id);
    if (entity == NULL)
        printf("[SERVICE] English created successfully\n");

    return entity;
}
EnglishEntity **englishServiceReadAll() {

    EnglishEntity **entities = englishReadAll();

    if (entities == NULL) {
        printf("[SERVICE] English table is empty");
        return NULL;
    }

    return entities;
}
int englishServiceUpdate(char *id, EnglishEntity entity) {

    if (id == NULL) {
        fprintf(stderr, "[SERVICE] Update failed: ID cannot be NULL\n");
        return -1;
    }

    int result = englishUpdate(id, entity);

    if (result == 0)
        printf("[SERVICE] English updated successfully\n");
    return result;
}
int englishServiceDelete(const char *id) {
    if (id == NULL) {
        fprintf(stderr, "[SERVICE] Delete failed: NULL ID\n");
        return -1;
    }

    int result = englishDelete(id);

    if (result > 0)
        printf("[SERVICE] English deleted successfully\n");

    return result;
}
