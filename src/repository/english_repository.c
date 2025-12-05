#include "../../headers/repository/english_repository.h"

#include <stdlib.h>
#include <string.h>

#include "../../headers/db/db.h"

int englishCreate(const EnglishEntity *entityToSave) {

    PGconn *conn = GLOBAL_DB_CONN;

    if (!conn) {
        fprintf(stderr, "DB connection not initialized\n");
        return -1;
    }

    char length_str[16];
    snprintf(length_str, sizeof(length_str), "%d", entityToSave->length);

    char listArray_str[1024] = {0};
    for (int i = 0; i < entityToSave->arraySize; i++) {
        char toAppend[20];
        if (i == 0) {
            snprintf(toAppend, sizeof(toAppend), "{%d,", entityToSave->listArray[i]);
            strcat(listArray_str, toAppend);
        }

        else if (i == entityToSave->arraySize - 1) {
            snprintf(toAppend, sizeof(toAppend), "%d}", entityToSave->listArray[i]);
            strcat(listArray_str, toAppend);
        }

        else {
            snprintf(toAppend, sizeof(toAppend), "%d,", entityToSave->listArray[i]);
            strcat(listArray_str, toAppend);
        }
    }
    const char *params[5] = {entityToSave->greet, entityToSave->kind ? "true" : "false", length_str,
                             entityToSave->object_json, listArray_str};

    PGresult *res = PQexecParams(conn, "INSERT INTO english (greet,kind,length,object,list) VALUES ($1,$2,$3,$4,$5);",
                                 5, NULL, params, NULL, NULL, 0);

    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        fprintf(stderr, "Error insert english: %s\n", PQerrorMessage(conn));
        PQclear(res);
        return -1;
    }

    PQclear(res);
    return 0;
}

EnglishEntity *englishRead(const char *id) {}

EnglishEntity **englishReadAll() {}

void englishUpdate(int id, EnglishEntity entityToUpdate) {}

int englishDelete(const char *id) {}
