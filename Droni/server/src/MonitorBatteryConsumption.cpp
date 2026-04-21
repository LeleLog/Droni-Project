#include "main.h"
// Controllo della batteria attuale del drone con confronto aspettativa consumo della batteria 
void MonitorBatteryConsumption(Con2DB db)
{
    PGresult *res; // Dichiarazione di un puntatore per contenere il risultato dell'esecuzione dei comandi SQL
    char sqlcmd[1024]; // Buffer per memorizzare i comandi SQL

    // Recupera i dati dei droni dal database
    sprintf(sqlcmd, "SELECT id, distance, battery FROM DroneLog"); // Comando SQL per selezionare id, distanza e batteria DroneLog
    res = db.ExecSQLtuples(sqlcmd); // Esegue il comando SQL e memorizza il risultato in res

    int rows = PQntuples(res); // Ottiene il numero di righe (drone) restituito dalla query
    for (int i = 0; i < rows; ++i) { // Cicla su ogni riga (drone) del risultato
        int drone_id = atoi(PQgetvalue(res, i, 0)); // Converte l'ID del drone in un intero
        double distance = atof(PQgetvalue(res, i, 1)); // Converte la distanza totale in un valore double
        double battery = atof(PQgetvalue(res, i, 2)); // Converte lo stato attuale della batteria in un valore double

        // Verifica se il consumo della batteria è coerente con la distanza percorsa
        int check_result = 0;  // 0 = OK, 1 = Errore
        
        if(distance != 0) { // Esegue il controllo solo se la distanza percorsa è diversa da 0
            double expected_battery = 100 - (distance / 0.25 * 1.667);  // Calcola la batteria attesa: ogni 250 metri si consuma il 1,667%
            if (battery < expected_battery - 5) {  // Controlla se la batteria attuale è inferiore di oltre il 5% rispetto a quella attesa
               check_result = 1;
         }
        }
        // Aggiorna il risultato del controllo nella tabella DroneLog
        sprintf(sqlcmd,
                "UPDATE DroneLog SET battery_consumption_check = %d WHERE id = %d",
                 check_result, drone_id); // Comando SQL per aggiornare il campo battery_consumption_check
        PGresult *res_update = db.ExecSQLcmd(sqlcmd); // Esegue il comando SQL per aggiornare il record nel database
        PQclear(res_update); // Libera la memoria associata al risultato della query di aggiornamento
    }

    PQclear(res); // Libera la memoria associata al risultato della query di selezione
}
