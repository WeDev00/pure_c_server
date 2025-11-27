#ifndef DB_CONNECTION_H
#define DB_CONNECTION_H

#include <libpq-fe.h>

/*
 *  PGconn is a libpq-fe type that identifies a connection
 */

extern PGconn *GLOBAL_DB_CONN;

int db_connect(const char *conninfo);

// used to close the connection to the database
void db_disconnect();

// used to verify that the database is OK: returns 1 in this case, 0 otherwise
int db_is_ok();

#endif
