#include "../../headers/service/italian_service.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../../headers/repository/italian_repository.h"

static int validateItalianEntity(const ItalianEntity *entity) {
    if (entity == NULL)
        return 0;
    if (strlen(entity->greet) == 0)
        return 0;
    if (strlen(entity->greet) > 45)
        return 0;
    return 1;
}

int italianServiceCreate(ItalianEntity entity) {
    if (!validateItalianEntity(&entity)) {
        fprintf(stderr, "[SERVICE] Validation failed (create Italian)\n");
        return -1;
    }

    int result = italianCreate(&entity);
    if (result == 0)
        printf("[SERVICE] Italian created successfully\n");

    return result;
}
ItalianEntity *italianServiceRead(const char *id) {
    if (id == NULL) {
        fprintf(stderr, "[SERVICE] ID cannot be NULL\n");
        return NULL;
    }

    ItalianEntity *ent = italianRead(id);

    if (ent == NULL) {
        printf("[SERVICE] Italian id '%s' not found\n", id);
        return NULL;
    }

    return ent;
}
ItalianEntity **italianServiceReadAll() {

    ItalianEntity **entities = italianReadAll();

    if (entities == NULL) {
        printf("[SERVICE] Italian table is empty");
        return NULL;
    }

    return entities;
}

void italianServiceUpdate(char *id, ItalianEntity entity) {
    if (id == NULL) {
        fprintf(stderr, "[SERVICE] Update failed: ID cannot be NULL\n");
        return;
    }

    if (!validateItalianEntity(&entity)) {
        fprintf(stderr, "[SERVICE] Update validation failed\n");
        return;
    }

    int result = italianUpdate(id, entity);

    if (result == 0)
        printf("[SERVICE] Italian updated successfully\n");
}
int italianServiceDelete(const char *id) {
    if (id == NULL) {
        fprintf(stderr, "[SERVICE] Delete failed: NULL ID\n");
        return -1;
    }

    int result = italianDelete(id);

    if (result == 0)
        printf("[SERVICE] Italian deleted successfully\n");

    return result;
}
