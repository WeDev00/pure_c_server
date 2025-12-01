#include "../../headers/repository/italian_repository.h"

#include <stdlib.h>

#include "../../headers/db/db.h"

int italianCreate(const ItalianEntity *rec) {
    PGconn *conn = GLOBAL_DB_CONN;

    if (!conn) {
        fprintf(stderr, "DB connection not initialized\n");
        return -1;
    }

    char length_str[16];
    snprintf(length_str, sizeof(length_str), "%d", rec->length);
    const char *params[3] = {rec->greet, rec->kind ? "true" : "false", length_str};

    /*
     * PQexecParams(
     * is the libpq function for executing a parameterized query (with placeholders $1, $2, ...). It is preferable to
     * concatenating strings because it avoids injection and simplifies value binding. The parameters are:
     * 1. conn, the pointer to the active connection (PGconn*). This is the connection through which the query is sent
     * to the server.
     * 2. “INSERT INTO Italian (Greet,Kind,Length) VALUES ($1,$2,$3);”, the SQL string to be executed. The
     *    placeholders $1,$2,$3 are replaced by the values provided in the params array.
     * 3. "3", the number of parameters (here 3). It must match the number of actual elements in params.
     * 4. NULL, an optional array of OIDs that specifies the type of each parameter (e.g., INT4OID, TEXTOID).
     *    Here, NULL means: let the server infer the type from the values/text sent. This is fine for text formats;
     *    if you use binary formats or want better performance/accuracy, you would specify the types.
     * 5. params, the array of pointers to strings with the text values to pass for $1,$2,$3. params[0]=rec->greet,
     *    params[1]=“true”/“false”, params[2]=length_str
     * 6. NULL, optional array of parameter lengths (useful only in binary format). For parameters in text format, you
     *    can pass NULL.
     * 7. NULL, optional array of parameter formats: 0 = text, 1 = binary. NULL is equivalent to all in text format.
     *    (If you want some binary parameters, you would put an array of 0/1 here.)
     * 8. result format: 0 = request to the db server to return the result in text format; 1 = binary format.
     */
    PGresult *res = PQexecParams(conn, "INSERT INTO italian (Greet,Kind,Length) VALUES ($1,$2,$3);", 3, NULL, params,
                                 NULL, NULL, 0);

    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        fprintf(stderr, "Error insert Italian: %s\n", PQerrorMessage(conn));
        PQclear(res);
        return -1;
    }

    PQclear(res);
    return 0;
}

ItalianEntity *italianRead(const char *id) {
    const char *params[1] = {id};
    PGconn *conn = GLOBAL_DB_CONN;
    PGresult *res = PQexecParams(conn, "SELECT * FROM italian WHERE Id = $1;", 1, NULL, params, NULL, NULL, 0);

    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        fprintf(stderr, "[italianRead] Query error: %s\n", PQerrorMessage(conn));
        PQclear(res);
        return NULL;
    }

    /*
     * PQntuples(res) counts how many rows res contains and only works with SQL commands that return tuples (such as the
     * SELECT command we just used). In the case of other commands (such as DELETE or UPDATE), “PQntuples” will return 0
     * because there are no tuples returned (but there will be tuples affected by these commands, which we will count
     * with PQcmdTuples).
     */
    int rows = PQntuples(res);

    if (rows == 0) {
        fprintf(stderr, "[italianRead] Query error: No result for the given id\n");
        PQclear(res);
        return NULL;
    }

    if (rows >= 2) {
        fprintf(stderr, "[italianRead] Query error: Too many result for the given id\n");
        PQclear(res);
        return NULL;
    }

    ItalianEntity *entity = malloc(sizeof(ItalianEntity));
    if (!entity) {
        fprintf(stderr, "[italianRead] malloc failed\n");
        PQclear(res);
        return NULL;
    }

    /*
     * PQgetvalue(res, tup_num, field_num) returns the value present in res at coordinates (row, column)
     * tup_num;field_num.
     * In our case, we have only one tuple from which to extract data, so tup_num will always be 0.  If we had more
     * tuples, we would have to iterate over “rows”.
     * The column to be extracted varies with each call:
     * In the first case, we want the "ID", which is the first column (so field_num = 0);
     * In the second case, we want "greet", which is the second column (so field_num = 1);
     *  In the third case, we want a Boolean, and PQgetvalue returns “t” if true and “f” if false.
     */
    snprintf(entity->id, sizeof(entity->id), "%s", PQgetvalue(res, 0, 0));
    snprintf(entity->greet, sizeof(entity->greet), "%s", PQgetvalue(res, 0, 1));

    // PQgetvalue ritorna "t" o "f" per booleani quando in formato testo
    entity->kind = (PQgetvalue(res, 0, 2)[0] == 't') ? 1 : 0;

    entity->length = atoi(PQgetvalue(res, 0, 3));

    PQclear(res);
    return entity;
}

ItalianEntity **italianReadAll() {
    PGconn *conn = GLOBAL_DB_CONN;
    PGresult *res = PQexecParams(conn, "SELECT * FROM italian;", 0, NULL, NULL, NULL, NULL, 0);

    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        fprintf(stderr, "[italianRead] Query error: %s\n", PQerrorMessage(conn));
        PQclear(res);
        return NULL;
    }

    int rows = PQntuples(res);
    if (rows <= 0) {
        return NULL;
    }

    ItalianEntity **entities = malloc(rows * sizeof(ItalianEntity *));
    if (!entities) {
        fprintf(stderr, "[italianRead] malloc failed\n");
        PQclear(res);
        return NULL;
    }

    for (int i = 0; i < rows; i++) {
        entities[i] = malloc(sizeof(ItalianEntity));
        snprintf(entities[i]->id, sizeof(entities[i]->id), "%s", PQgetvalue(res, i, 0));
        snprintf(entities[i]->greet, sizeof(entities[i]->greet), "%s", PQgetvalue(res, i, 1));
        entities[i]->kind = (PQgetvalue(res, i, 2)[0] == 't') ? 1 : 0;
        entities[i]->length = atoi(PQgetvalue(res, i, 3));
    }
    entities[rows] = NULL;
    PQclear(res);
    return entities;
}

int italianUpdate(char *id, ItalianEntity entity) {
    PGconn *conn = GLOBAL_DB_CONN;
    const char *params[4];
    params[0] = entity.greet;
    params[1] = entity.kind ? "t" : "f";
    char length_buf[12];

    snprintf(length_buf, sizeof(length_buf), "%d", entity.length);
    params[2] = length_buf;

    char id_buf[37];
    snprintf(id_buf, sizeof(id_buf), "%s", id);
    params[3] = id_buf;

    PGresult *res = PQexecParams(conn, "UPDATE italian SET greet=$1, kind=$2, length=$3 WHERE ID=$4;", 4, NULL, params,
                                 NULL, NULL, 0);

    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        fprintf(stderr, "Error updating Italian: %s\n", PQerrorMessage(conn));
        PQclear(res);
        return -1;
    }

    PQclear(res);
    return 0;
}

int italianDelete(const char *id) {
    PGconn *conn = GLOBAL_DB_CONN;
    const char *params[1];
    params[0] = id;

    PGresult *res = PQexecParams(conn, "DELETE FROM italian WHERE ID=$1;", 1, NULL, params, NULL, NULL, 0);

    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        fprintf(stderr, "Error deleting Italian: %s\n", PQerrorMessage(conn));
        PQclear(res);
        return -1;
    }

    int affected = atoi(PQcmdTuples(res)); // numero di righe eliminate

    PQclear(res);

    return affected;
}
