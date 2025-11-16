#ifndef DB_CONNECTION_H
#define DB_CONNECTION_H

#include <libpq-fe.h>

/* create a "DBConnection" type that identifies a pointer to the connection
 * where PGconn is a libpq-fe type that identifies a connection
 */
typedef struct {
    PGconn *conn;
} DBConnection;

/*
* db_connect()
     |
     v
   DBConnection*
       |
       +--> struct DBConnection
                |
                +--> PGconn* conn
Function used to connect to the database server starting from conninfo
 */

DBConnection *db_connect(const char *conninfo);

// used to close the connection to the database
void db_disconnect(DBConnection *db);

// used to verify that the database is OK: returns 1 in this case, 0 otherwise
int db_is_ok(DBConnection *db);

#endif
