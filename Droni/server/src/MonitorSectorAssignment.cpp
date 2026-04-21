#include "main.h"
// Controllo della posizione dei droni all'interno del settore assegnato
void MonitorSectorAssignment(Con2DB db)
{
    PGresult *res; // Dichiarazione di un puntatore per contenere il risultato dell'esecuzione dei comandi SQL
    char sqlcmd[1024]; // Buffer per memorizzare i comandi SQL

    // Recupera i dati dei droni dal database
    sprintf(sqlcmd, "SELECT id, pos_x, pos_y, sector FROM DroneLog"); // Comando SQL per selezionare id, pos_x, pos_y e settore da DroneLog
    res = db.ExecSQLtuples(sqlcmd); // Esegue il comando SQL e memorizza il risultato in res

    int rows = PQntuples(res); // Ottiene il numero di righe (drone) restituito dalla query
    for (int i = 0; i < rows; ++i) { // Cicla su ogni riga (drone) del risultato
        int drone_id = atoi(PQgetvalue(res, i, 0)); // Converte l'ID del drone in un intero
        double pos_x = atof(PQgetvalue(res, i, 1)); // Converte la posizione X del drone in un double
        double pos_y = atof(PQgetvalue(res, i, 2)); // Converte la posizione Y del drone in un double
        int settore = atoi(PQgetvalue(res, i, 3)); // Converte il settore assegnato del drone in un intero

        // Verifica che il drone sia all'interno del settore
        int check_result = 0;  // 0 = OK, 1 = Errore
        
        if(settore != -1) { // Esegue il controllo solo se il settore è assegnato (valore diverso da -1)
        
           // Calcola le coordinate del settore. Un settore è un'area 1x1 in una griglia 6x6
            int sector_x = settore % 6; // Calcola la coordinata X del settore (colonna)
            int sector_y = settore / 6; // Calcola la coordinata Y del settore (riga)
            // Verifica se le coordinate del drone sono all'interno dei limiti del settore
            if (pos_x < sector_x || pos_x > sector_x + 1 || pos_y < sector_y || pos_y > sector_y + 1) {
               check_result = 1;
            }
        }    

        // Aggiorna il risultato del controllo nella tabella DroneLog
        sprintf(sqlcmd,
                "UPDATE DroneLog SET sector_assignment_check = %d WHERE id = %d",
                 check_result, drone_id); // Comando SQL per aggiornare il campo sector_assignmenet_check
        PGresult *res_update = db.ExecSQLcmd(sqlcmd); // Esegue il comando SQL per aggiornare il record nel database
        PQclear(res_update); // Libera la memoria associata al risultato della query di aggiornamento
    }

    PQclear(res); // Libera la memoria associata al risultato della query di selezione
}
