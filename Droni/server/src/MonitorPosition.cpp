#include "main.h"
// Controllo della posizione dei droni all'interno dell'area di monitoraggio
void MonitorPosition(Con2DB db)
{
    PGresult *res; // Dichiarazione di un puntatore per contenere il risultato dell'esecuzione dei comandi SQL
    char sqlcmd[1024]; // Buffer per memorizzare i comandi SQL

    // Recupera i dati dei droni dal database
    sprintf(sqlcmd, "SELECT id, pos_x, pos_y FROM DroneLog"); // Comando SQL per selezionare id, pos_x e pos_y da DroneLog
    res = db.ExecSQLtuples(sqlcmd); // Esegue il comando SQL e memorizza il risultato in res

    int rows = PQntuples(res); // Ottiene il numero di righe (drone) restituito dalla query
    for (int i = 0; i < rows; ++i) { // Cicla su ogni riga (drone) del risultato
        int drone_id = atoi(PQgetvalue(res, i, 0)); // Converte l'ID del drone in un intero (atoi = ASCII to integer)
        double pos_x = atof(PQgetvalue(res, i, 1)); // Converte la posizione X del drone in un double
        double pos_y = atof(PQgetvalue(res, i, 2)); // Converte la posizione Y del drone in un double

        // Verifica se il drone è fuori dall'area definita (6x6 unità)
        int check_result = 0;  // 0 = OK, 1 = Errore
        if (pos_x < 0 || pos_x > 6 || pos_y < 0 || pos_y > 6) {
            check_result = 1;
        }

        // Aggiorna il risultato del controllo nella tabella DroneLog
        sprintf(sqlcmd,
                 "UPDATE DroneLog SET position_check = %d WHERE id = %d ",
                 check_result, drone_id); // Comando SQL per aggiornare il campo position_check
        PGresult *res_update = db.ExecSQLcmd(sqlcmd); // Esegue il comando SQL per aggiornare il record nel database
        PQclear(res_update); // Libera la memoria associata al risultato della query di aggiornamento
    }

    PQclear(res); // Libera la memoria associata al risultato della query di selezione
}
