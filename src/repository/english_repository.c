#include "../../headers/repository/english_repository.h"
#include <stdlib.h>
#include <string.h>
#include "../../headers/db/db.h"
#include "../../headers/utility/utility.h"

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

EnglishEntity *englishRead(const char *id) {

    const char *params[1] = {id};
    PGconn *conn = GLOBAL_DB_CONN;
    PGresult *res = PQexecParams(conn, "SELECT * FROM english WHERE Id = $1;", 1, NULL, params, NULL, NULL, 0);

    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        fprintf(stderr, "[EnglishRead] Query error: %s\n", PQerrorMessage(conn));
        PQclear(res);
        return NULL;
    }

    int rows = PQntuples(res);

    if (rows == 0) {
        fprintf(stderr, "[EnglishRead] Query error: No result for the given id\n");
        PQclear(res);
        return NULL;
    }

    if (rows >= 2) {
        fprintf(stderr, "[EnglishRead] Query error: Too many result for the given id\n");
        PQclear(res);
        return NULL;
    }

    EnglishEntity *entity = malloc(sizeof(EnglishEntity));
    if (!entity) {
        fprintf(stderr, "[EnglishRead] malloc failed\n");
        PQclear(res);
        return NULL;
    }

    snprintf(entity->id, sizeof(entity->id), "%s", PQgetvalue(res, 0, 0));
    snprintf(entity->greet, sizeof(entity->greet), "%s", PQgetvalue(res, 0, 1));
    entity->kind = (PQgetvalue(res, 0, 2)[0] == 't') ? 1 : 0;
    entity->length = atoi(PQgetvalue(res, 0, 3));
    entity->object_json = malloc(1024 * sizeof(char));
    snprintf(entity->object_json, 1024, PQgetvalue(res, 0, 4));
    const char *closeCurlyBracketPosition = findNth(entity->object_json, '}', 1);
    int length = closeCurlyBracketPosition - entity->object_json;
    entity->object_json[length + 1] = '\0';
    int count = 0;
    char *arrayAsString = PQgetvalue(res, 0, 5);
    char *copyToCount = strdup(arrayAsString); // creation of a result copy to count elements in the array
    char *token = strtok(copyToCount, ",");

    while (token != NULL) {
        count++;
        token = strtok(NULL, ",");
    }
    entity->arraySize = count;
    arrayAsString++; // delete "{"
    arrayAsString[strlen(arrayAsString) - 1] = '\0'; // delete "}"
    entity->listArray = malloc(count * sizeof(int));
    int i = 0;
    token = strtok(arrayAsString, ",");
    while (token != NULL) {
        entity->listArray[i] = atoi(token);
        i++;
        token = strtok(NULL, ",");
    }
    PQclear(res);
    return entity;
}

EnglishEntity **englishReadAll() {
    PGconn *conn = GLOBAL_DB_CONN;
    PGresult *res = PQexecParams(conn, "SELECT * FROM english;", 0, NULL, NULL, NULL, NULL, 0);

    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        fprintf(stderr, "[englishReadAll] Query error: %s\n", PQerrorMessage(conn));
        PQclear(res);
        return NULL;
    }

    int rows = PQntuples(res);
    if (rows <= 0) {
        return NULL;
    }

    EnglishEntity **entities = malloc(rows * sizeof(EnglishEntity *));
    if (!entities) {
        fprintf(stderr, "[englishReadAll] malloc failed\n");
        PQclear(res);
        return NULL;
    }
    for (int i = 0; i < rows; i++) {
        entities[i] = malloc(sizeof(EnglishEntity));
        snprintf(entities[i]->id, sizeof(entities[i]->id), "%s", PQgetvalue(res, i, 0));
        snprintf(entities[i]->greet, sizeof(entities[i]->greet), "%s", PQgetvalue(res, i, 1));
        entities[i]->kind = (PQgetvalue(res, i, 2)[0] == 't') ? 1 : 0;
        entities[i]->length = atoi(PQgetvalue(res, i, 3));
        entities[i]->object_json = malloc(1024 * sizeof(char));
        snprintf(entities[i]->object_json, 1024, PQgetvalue(res, i, 4));
        const char *closeCurlyBracketPosition = findNth(entities[i]->object_json, '}', 1);
        int length = closeCurlyBracketPosition - entities[i]->object_json;
        entities[i]->object_json[length + 1] = '\0';
        int count = 0;
        char *arrayAsString = PQgetvalue(res, i, 5);
        char *copyToCount = strdup(arrayAsString); // creation of a result copy to count elements in the array
        char *token = strtok(copyToCount, ",");

        while (token != NULL) {
            count++;
            token = strtok(NULL, ",");
        }
        entities[i]->arraySize = count;
        arrayAsString++; // delete "{"
        arrayAsString[strlen(arrayAsString) - 1] = '\0'; // delete "}"
        entities[i]->listArray = malloc(count * sizeof(int));
        int j = 0;
        token = strtok(arrayAsString, ",");
        while (token != NULL) {
            entities[i]->listArray[j] = atoi(token);
            j++;
            token = strtok(NULL, ",");
        }
    }
    entities[rows] = NULL;
    PQclear(res);
    return entities;
}
int englishUpdate(char *id, EnglishEntity entity) {
    PGconn *conn = GLOBAL_DB_CONN;
    const char *params[6];
    params[0] = entity.greet;
    params[1] = entity.kind ? "t" : "f";
    char length_buf[12];

    snprintf(length_buf, sizeof(length_buf), "%d", entity.length);
    params[2] = length_buf;

    params[3] = entity.object_json;
    char *integerListAsString = malloc(1024 * sizeof(char));
    int averageSizePerElement = 1024 / entity.arraySize;
    for (int i = 0; i < entity.arraySize; i++) {
        int number = entity.listArray[i];
        if (i == 0) {
            snprintf(integerListAsString, averageSizePerElement, "{%d,", number);
        } else if (i == entity.arraySize - 1) {
            char *toAttach = malloc(averageSizePerElement * sizeof(char));
            snprintf(toAttach, averageSizePerElement, "%d", number);
            strcat(integerListAsString, toAttach);
            free(toAttach);
        } else {
            char *toAttach = malloc(averageSizePerElement * sizeof(char));
            snprintf(toAttach, averageSizePerElement, "%d,", number);
            strcat(integerListAsString, toAttach);
            free(toAttach);
        }
    }
    strcat(integerListAsString, "}");
    params[4] = integerListAsString;
    char id_buf[37];
    snprintf(id_buf, sizeof(id_buf), "%s", id);
    params[5] = id_buf;

    PGresult *res =
            PQexecParams(conn, "UPDATE english SET greet=$1, kind=$2, length=$3, object=$4, list=$5 WHERE ID=$6;", 6,
                         NULL, params, NULL, NULL, 0);

    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        fprintf(stderr, "Error updating english: %s\n", PQerrorMessage(conn));
        PQclear(res);
        return -1;
    }

    PQclear(res);
    return 0;
}

int englishDelete(const char *id) {}
