#include "main.h"

// Controlla se il tempo di ricarica di ciascun drone è superiore a 180 minuti
void MonitorRechargeTime(Con2DB db)
{
    PGresult *res; // Dichiarazione di un puntatore per contenere il risultato dell'esecuzione dei comandi SQL
    char sqlcmd[1024]; // Buffer per memorizzare i comandi SQL

    // Recupera i dati dei droni dal database
    sprintf(sqlcmd, "SELECT id, rechargetime FROM DroneLog");  // Comando SQL per selezionare id e il tempo di ricarica da DroneLog
    res = db.ExecSQLtuples(sqlcmd); // Esegue il comando SQL e memorizza il risultato in res

    int rows = PQntuples(res); // Ottiene il numero di righe (drone) restituito dalla query
    for (int i = 0; i < rows; ++i) { // Cicla su ogni riga (drone) del risultato
        int drone_id = atoi(PQgetvalue(res, i, 0)); // Converte l'ID del drone in un intero
        double recharge_time = atof(PQgetvalue(res, i, 1)); // Converte il tempo di ricarica in un double

        // Verifica se il tempo di ricarica è superiore a 180 minuti
        int check_result = (recharge_time > 180) ? 1 : 0;

        // Aggiorna il risultato del controllo nella tabella DroneLog
        sprintf(sqlcmd,
                "UPDATE DroneLog SET recharge_time_check = %d WHERE id = %d",
                check_result, drone_id);  // Comando SQL per aggiornare il campo recharge_time_check
        PGresult *res_update = db.ExecSQLcmd(sqlcmd); // Esegue il comando SQL per aggiornare il record nel database
        PQclear(res_update); // Libera la memoria associata al risultato della query di aggiornamento
    }

    PQclear(res); // Libera la memoria associata al risultato della query di selezione
}
