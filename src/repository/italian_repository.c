#include "../../headers/repository/italian_repository.h"
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

ItalianEntity *italianRead(const char *id) {}

void italianUpdate(int id, ItalianEntity entity) {}

int italianDelete(const char *id) {}
