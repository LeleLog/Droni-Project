#ifndef CONTROLCENTER_H
#define CONTROLCENTER_H

#include "../../client/src/drone.h"
#include "../../../con2db/pgsql.h"
#include "../../../con2redis/src/con2redis.h"
#include <main.h>


#define READ_STREAM "stream1"
#define WRITE_STREAM "stream2"

// Dichiarazione della classe ControlCenter
class ControlCenter {
public:
    // Costruttore della classe ControlCenter
    ControlCenter(double areaSize); 

    // Aggiorna lo stato dei droni
    void updateDrones();

    // Visualizza le specifiche di ogni drone 
    void printDroneStatus() const;

    // Inizializza i droni nei rispettivi settori
    void initializeDronesInSectors(); //

    // Sostituisce un drone in ricarica con uno di riserva
    void replaceDrone(Drone &drone, int sector);

    // Restituisce un tempo di ricarica casuale
    double getRandomRechargeTime() const;

    // Gestisce le code dei droni nei settori
    void manageDroneQueues();

    // Aggiorna la posizione del drone
    void updatePosition(Drone &drone);

    // Ritorno del drone alla base 
    void returnToBase(Drone &drone);

    // Esegue la ricarica del drone
    void recharge(Drone &drone);

    // Imposta il tempo di ricarica del drone
    void setRechargeTime(Drone &drone, double rechargeTime);
    
    // Calcola la posizione che deve assumere il nuovo drone attivo 
    std::pair<double,double> calculateNewPositionInSector(double preX, double prevY, double distance, int sector, double distanceSector);
    
    // Inizializza il database dei log
    void init_logDB(Con2DB db);
    
    // Registra i log nel database
    void log2DB(Con2DB db);
    
    // Invia i dati dei droni attivi al client
    void sendingDataToClient(Drone& drone);
    
    // Legge conferma dal client
    void readConfirmationFromClient();
    
    // Inizializza la connessione Redis
    void initRedisConnection();
    
    // Dorme per il numero di microsecondi richiesto
    int server_micro_sleep(long usec);
    
    // Termina connessione con Redis
    void finishConnectionRedis();
    
private:

    double baseX, baseY; // Posizione del centro di controllo
    int numSectors; // Numero di settori
    int gridSize; // Dimensione della griglia
    double maxDistanceToBase; // Distanza massima dal punto (0,0) al centro di controllo
    double batteryThreshold; // Soglia di batteria da mantenere per il rientro al centro di controllo
    
    redisContext* c2r; // Dichiarazione contesto Redis
    redisReply* reply; // Dichiarazione risposta Redis
    char username[100]; // Buffer per il nome utente
    int read_counter; // Contatore messaggi letti
    int send_counter; // Contatore messaggi inviati
    int block; // Timeout per il blocco della lettura in microsecondi
    int pid; // ID del processo
    unsigned int seed; // Seed per la generazione casuale

    std::vector<Drone> drones; // Vettore di droni attivi
    std::vector<Drone> reserveDrones; // Vettore di droni di riserva
    std::vector<Drone> chargingDrones; // Vettore dei droni in ricarica
    std::vector<std::queue<int>> sectorQueues; // Code di droni per ogni settore
    

};

#endif // CONTROLCENTER_H
