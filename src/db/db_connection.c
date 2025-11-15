#include "../../headers/db/db_connection.h"
#include <stdio.h>
#include <stdlib.h>

DBConnection *db_connect(const char *conninfo) {
    // struttura che conterrà la connessione
    DBConnection *db = malloc(sizeof(DBConnection));
    if (!db)
        return NULL;

    // inizializziamo la connessione, memorizzando le info nel campo conn di DBConnection
    db->conn = PQconnectdb(conninfo);

    if (PQstatus(db->conn) != CONNECTION_OK) {
        /*
         * Affinchè la connessione vada a buon fine, il server di DB deve essere avviato
         * Per avviarlo, runnare il comando
& "C:\Program Files\PostgreSQL\{version}\bin\pg_ctl.exe" start -D "C:\Program Files\PostgreSQL\{version}\data" -w
         * su PowerShell, dove "version" è la versione di postgreSQL installata Per
         * stopparlo runnare
         * "& "C:\Program Files\PostgreSQL\{version}\bin\pg_ctl.exe" stop -D "C:\Program
         * Files\PostgreSQL\{version}\data" -w" sempre su PowerShell
         *
         * In alternativa, si può modificare il CMakeList per aggiungere un comando
         * add_custom_command(TARGET pure_c_server POST_BUILD
         *   COMMAND "C:/Program Files/PostgreSQL/17/bin/pg_ctl.exe" start -D "C:/Program Files/PostgreSQL/17/data"
         *   COMMAND ${CMAKE_COMMAND} -E echo "PostgreSQL avviato"
         *   COMMAND $<TARGET_FILE:pure_c_server>
         *  )
         *
         *  Oppure, si può creare uno script batch che prima avvia il server di db e poi quello C
         *
         *
         *  Per controllare lo stato del server di db, verificare lo stato del servizio "postgresql-x64-{version}"
         *  tramite "Win+R -> services.msc"
         */
        fprintf(stderr, "[DB] Connessione fallita: %s\n", PQerrorMessage(db->conn));
        PQfinish(db->conn);
        free(db);
        return NULL;
    }
    printf("[DB] Connessione PostgreSQL stabilita.\n");
    return db;
}

void db_disconnect(DBConnection *db) {
    if (!db)
        return;
    PQfinish(db->conn);
    free(db);
    printf("[DB] Connessione PostgreSQL chiusa.\n");
}

int db_is_ok(DBConnection *db) { return db && db->conn && PQstatus(db->conn) == CONNECTION_OK; }
