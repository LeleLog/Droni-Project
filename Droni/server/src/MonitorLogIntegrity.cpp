#include "main.h"
// Controllo dell'integrità del log verificando se ci sono voci duplicate per ogni drone nel db, attraverso il campo id
void MonitorLogIntegrity(Con2DB db)
{
    PGresult *res; // Dichiarazione di un puntatore per contenere il risultato dell'esecuzione dei comandi SQL
    char sqlcmd[1024]; // Buffer per memorizzare i comandi SQL

    // Verifica duplicati basati su id (ci dovrebbe essere solo una voce per drone)
    sprintf(sqlcmd, "SELECT id, COUNT(*) FROM DroneLog GROUP BY id HAVING COUNT(*) > 1");
    // Crea il comando SQL per raggruppare le voci nel log per id del drone e seleziona quelli che hanno più di un record (duplicato)
    res = db.ExecSQLtuples(sqlcmd); // Esegue il comando SQL e memorizza il risultato in res

    int rows = PQntuples(res); // Ottiene il numero di righe (duplicati) restituiti dalla query
    for (int i = 0; i < rows; ++i) { // Cicla su ogni (drone duplicato) del risultato
        int drone_id = atoi(PQgetvalue(res, i, 0)); // Converte l'ID del drone in un intero
        int check_result = 1;  // Errore, c'è un duplicato

        // Aggiorna il risultato del controllo nella tabella DroneLog
        sprintf(sqlcmd,
                "UPDATE DroneLog SET log_integrity_check = %d WHERE id = %d",
                 check_result, drone_id); // Comando SQL per aggiornare il campo log_integrity_check
        PGresult *res_update = db.ExecSQLcmd(sqlcmd); // Esegue il comando SQL per aggiornare il record nel database
        PQclear(res_update); // Libera la memoria associata al risultato della query di aggiornamento
    }

    PQclear(res); // Libera la memoria associata al risultato della query di selezione
}
