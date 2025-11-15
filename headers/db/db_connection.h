#ifndef DB_CONNECTION_H
#define DB_CONNECTION_H

#include <libpq-fe.h>

/* creiamo un tipo "DBConnection" che identifica un puntatore alla connessione
 * dove PGconn è proprio un tipo di libpq-fe che identifica una connessione
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
Funzione che serve a connetterci al server di db partendo dalle conninfo
 */

DBConnection *db_connect(const char *conninfo);

// serve a chiudere la connessione con il db
void db_disconnect(DBConnection *db);

// serve a verificare che il db sia OK: ritorna 1 in questo caso, 0 altrimenti
int db_is_ok(DBConnection *db);

#endif
