#include "../../headers/db/db.h"
#include <stdio.h>
#include <stdlib.h>

PGconn *GLOBAL_DB_CONN = NULL;

int db_connect(const char *conninfo) {


    // initialize the connection, storing the information in the conn field of DBConnection.
    GLOBAL_DB_CONN = PQconnectdb(conninfo);

    if (PQstatus(GLOBAL_DB_CONN) != CONNECTION_OK) {
        /*
         * For the connection to succeed, the DB server must be started
         * [The Postgres service starts automatically when the PC is turned on. To change this, simply run the
         * services.msc command to open the Windows Services panel. Then search for postgresql-x64-{version} and go to
         * the service properties to change the startup type from Automatic to Manual.]
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
        fprintf(stderr, "[DB] Connection failed: %s\n", PQerrorMessage(GLOBAL_DB_CONN));
        PQfinish(GLOBAL_DB_CONN);
        free(GLOBAL_DB_CONN);
        return -1;
    }
    printf("[DB] PostgreSQL connection established.\n");
    return 0;
}

void db_disconnect() {
    if (!GLOBAL_DB_CONN)
        return;
    PQfinish(GLOBAL_DB_CONN);
    GLOBAL_DB_CONN = NULL;
    printf("[DB] PostgreSQL connection closed.\n");
}

int db_is_ok() { return GLOBAL_DB_CONN && PQstatus(GLOBAL_DB_CONN) == CONNECTION_OK; }
