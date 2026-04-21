#include "main.h"

int main() {
    
    // Definisce un separatore da usare nella stampa dello stato dei droni
    const char* separator = "__________________________________________________________________________\n";
    
    // Configurazione dell'output per il debug
#if (DEBUG > 0)
    // Disabilita il buffering per stdout e stderr per stampare immediatamente
    setvbuf(stdout, (char *)NULL, _IONBF, 0);
    setvbuf(stderr, (char *)NULL, _IONBF, 0);
#endif

    // Inizializzazione del centro di controllo con un'area di 36 km^2
    double areaSize = 36.0;
    ControlCenter controlCenter(areaSize); // Inizializza il centro di controllo con la dimensione dell'area
    controlCenter.printDroneStatus(); // Stampa generale dello stato iniziale dei droni 
    printf("%s", separator); // Stampa separatore
    // Inizializza l'istanza del tempo del programma
    Time time_program;
    // Inizializza la connessione al database
    Con2DB db1("localhost", "5432", "controlcenter_user", "47002","logdb_controlcenter");
    controlCenter.init_logDB(db1); // Inizializza il database con i dati iniziali 
    // Inizializza la connesione a Redis
    controlCenter.initRedisConnection();
    
    // Inizializza i droni nei rispettivi settori
    controlCenter.initializeDronesInSectors();
    // Stampa generale dello stato dei droni dopo l'inizializzazione nei settori
    controlCenter.printDroneStatus();
    printf("%s", separator); // Stampa separatore
    
    // Ciclo principale del programma, esegue fino a quando non raggiunge l'orizzonte temporale definito
    while (time_program.getTime_Step() < HORIZON) {
        
        time_program.print_time_program(); // Stampa le informazioni temporali del programma
        controlCenter.updateDrones(); // Aggiorna lo stato dei droni
        controlCenter.printDroneStatus(); // Stampa generale dello stato dei droni dopo un passo temporale
        printf("%s", separator); // Stampa separatore
        
        // Registra i dati aggioranti (log) dei droni nel database
        controlCenter.log2DB(db1); 
        // Monitora i dati nel database
        MonitorPosition(db1);           // Verifica la posizione dei droni 
        MonitorSectorAssignment(db1);   // Verifica l'assegnazione dei settori
        MonitorBatteryConsumption(db1); // Verifica il consumo di batteria
        MonitorRechargeTime(db1);       // Verifica il tempo di ricarica
        MonitorLogIntegrity(db1);       // Veirfica l'integrità dei log
        
        // Mette in pausa l'esecuzione per il periodo indicato in microsecondi
        controlCenter.server_micro_sleep(100000);
        // Aggiorna il tempo di simulazione
        time_program.update_time();
    }
    
    // Chiude la connesione Redis
    controlCenter.finishConnectionRedis();
}







