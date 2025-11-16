#include "../../headers/db/db_connection.h"
#include <stdio.h>
#include <stdlib.h>

DBConnection *db_connect(const char *conninfo) {
    // structure that will contain the connection
    DBConnection *db = malloc(sizeof(DBConnection));
    if (!db)
        return NULL;

    // initialize the connection, storing the information in the conn field of DBConnection.
    db->conn = PQconnectdb(conninfo);

    if (PQstatus(db->conn) != CONNECTION_OK) {
        /*
         * For the connection to succeed, the DB server must be started
         * To start it, run the command
         * Start-Service -Name postgresql-x64-{version}
         * on PowerShell RUN AS ADMINISTRATOR, where "version" is the installed postgreSQL version
         * To stop it, run
         * Stop-Service -Name postgresql-x64-{version}
         * also on PowerShell
         *
         * Alternatively, you can modify the CMakeList to add a command
         * add_custom_command(TARGET pure_c_server POST_BUILD
         *   COMMAND "C:/Program Files/PostgreSQL/17/bin/pg_ctl.exe" start -D "C:/Program Files/PostgreSQL/17/data"
         *   COMMAND ${CMAKE_COMMAND} -E echo "PostgreSQL started"
         *   COMMAND $<TARGET_FILE:pure_c_server>
         *  )
         *
         *  Or, you can create a batch script that first starts the db server and then the C server
         *
         *
         *  To check the db server status, verify the status of the "postgresql-x64-{version}" service
         *  through "Win+R -> services.msc"
         */
        fprintf(stderr, "[DB] Connection failed: %s\n", PQerrorMessage(db->conn));
        PQfinish(db->conn);
        free(db);
        return NULL;
    }
    printf("[DB] PostgreSQL connection established.\n");
    return db;
}

void db_disconnect(DBConnection *db) {
    if (!db)
        return;
    PQfinish(db->conn);
    free(db);
    printf("[DB] PostgreSQL connection closed.\n");
}

int db_is_ok(DBConnection *db) { return db && db->conn && PQstatus(db->conn) == CONNECTION_OK; }
