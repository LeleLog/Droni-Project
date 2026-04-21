
#include "main.h"


// Implementazione del costruttore della classe ControlCenter
ControlCenter::ControlCenter(double areaSize) : c2r(nullptr), reply(nullptr), read_counter(0), send_counter(0), block(1000000000), pid(0), seed(0) {
    baseX = baseY = sqrt(areaSize) / 2.0; // Imposta la posizione del centro di controllo al centro dell'area
    numSectors = static_cast<int>(areaSize); // Calcola il numero di settori 
    gridSize = static_cast<int>(sqrt(numSectors)); // Dimensione della griglia (6 x 6)
    int numDrones = numSectors * 4; // 4 droni per settore

    sectorQueues.resize(numSectors); // Ridimensiona le code di settori
    drones.reserve(numDrones); // Riserva spazio per i droni operativi
    reserveDrones.reserve(numDrones+1); // Riserva spazio per i droni di riserva

    maxDistanceToBase = sqrt(baseX * baseX + baseY * baseY); // Distanza massima dal punto (0,0) al centro di controllo
    batteryThreshold = (maxDistanceToBase * (100 / ( 30.0 * 0.5 ))) + 1.66; // Calcola la soglia di sicurezza per il ritorno alla base (con 100 intera capacità della batteria in percentuale, 30 autonomia in minuti, 0,5 velocità di volo espressa in km/min e aggiunta soglia di consumo per 250m di sicurezza)

    for (int i = 0; i < numDrones; ++i) {
        Drone drone(i, baseX, baseY, 100.0); // Inizializza un drone con ID, posizione e durata della batteria
        drone.setSector(-1); // Imposta il settore a -1
        drones.push_back(drone); // Aggiunge il drone al vettore dei droni operativi

        // Inizializza un drone di riserva
        Drone reserveDrone(i + numDrones, baseX, baseY, 100.0); // Inizializza un drone di riserva
        reserveDrone.setSector(-1); // Imposta il settore a -1
        reserveDrones.push_back(reserveDrone); // Aggiunge il drone al vettore dei droni di riserva
         
    }

}


// Inizializza il database dei log
void ControlCenter::init_logDB(Con2DB db) {

    PGresult *res; // Dichiarazione di un puntatore per contenere il risultato dell'esecuzione dei comandi SQL
    char sqlcmd[1024]; // Buffer per memorizzare i comandi SQL
    
    //Inizia una transazione
    sprintf(sqlcmd, "BEGIN"); // Prepara il comando SQL per avviare una transazione
    res = db.ExecSQLcmd(sqlcmd); // Esegue il comando SQL
    PQclear(res); // Libera la memoria allocata per il risultato
    
    // Crea la tabella DroneLog con tutte le colonne necessarie
    sprintf(sqlcmd,
       "CREATE TABLE IF NOT EXISTS DroneLog ("
       "id INT PRIMARY KEY, "
       "pos_x DOUBLE PRECISION NOT NULL, "
       "pos_y DOUBLE PRECISION NOT NULL, "
       "sector INT NOT NULL, "
       "distance DOUBLE PRECISION NOT NULL, "
       "battery DOUBLE PRECISION NOT NULL, "
       "rechargetime DOUBLE PRECISION NOT NULL, "
       "pid INT NOT NULL, "
       "time TIMESTAMP DEFAULT CURRENT_TIMESTAMP, "
       "position_check INT DEFAULT 0, "
       "sector_assignment_check INT DEFAULT 0, "
       "battery_consumption_check INT DEFAULT 0, "
       "recharge_time_check INT DEFAULT 0, "
       "log_integrity_check INT DEFAULT 0)"
    );
    res = db.ExecSQLcmd(sqlcmd); // Esegue il comando SQL per creare la tabella
    PQclear(res); // Libera la memoria allocata per il risultato
    
    // Conclude la transazione 
    sprintf(sqlcmd, "COMMIT"); // Prepara il comando SQL per confermare la transazione
    res = db.ExecSQLcmd(sqlcmd); // Esegue il comando SQL
    PQclear(res); // Libera la memoria allocata per il risultato

}


// Inizializza la connessione Redis
void ControlCenter::initRedisConnection() {
     
     // Inizializza il seed per la generazione dei numeri casuali
     seed = (unsigned)time(NULL);
     srand(seed);
     
     // Genera un nome utente casuale per il processo
     sprintf(username, "%u", rand());
     
     // Ottiene l'ID del processo corrente
     pid = getpid();
    
     // Connessione al server Redis  
     printf("main(): pid %d: user %s: seed %d: connecting to redis ...\n", pid, username, seed);
     c2r = redisConnect("localhost", 6379); // Apre una connessione con il serve Redis in esecuzione su localhost alla porta 6379
     printf("main(): pid %d: user %s: seed %d: connected to redis\n", pid, username, seed);

     // Cancellazione degli stream esistenti su Redis (READ_STREAM e WRITE_STREAM) per assicurarci di non avere dati residui negli stream
     reply = RedisCommand(c2r, "DEL %s", READ_STREAM);
     assertReply(c2r, reply); // Verifica che la risposta non sia NULL
     dumpReply(reply, 0); // Stampa i dettagli della risposta

     reply = RedisCommand(c2r, "DEL %s", WRITE_STREAM);
     assertReply(c2r, reply); // Verifica che la risposta non sia NULL
     dumpReply(reply, 0); // Stampa i dettagli della risposta

     // Inizializzazione degli stream di lettura e scrittura
     initStreams(c2r, READ_STREAM);
     initStreams(c2r, WRITE_STREAM);

}


// Inizializza i droni nei rispettivi settori
void ControlCenter::initializeDronesInSectors() {

    // Definisce quanti droni saranno assegnati per settore
    int dronesPerSector = 4;
    
    // Cicla attraverso tutti i droni disponibili
    for (size_t i = 0; i < drones.size(); ++i) {
        int sectorIndex = i / dronesPerSector; // Calcola l'indice del settore a cui appartiene il drone
        int droneIndex = i % dronesPerSector; // Determina la posizione relativa del drone all'interno del settore
        int row = sectorIndex / gridSize; // Calcola la riga della griglia basandosi sull'indice del settore
        int col = sectorIndex % gridSize; // Calcola la colonna della griglia basandosi sull'indice del settore
        double targetX = col; // Determina la posizione X di destinazione del drone nel settore (colonna * 1.0)
        double targetY = row; // Determina la posizione Y di destinazione del drone nel settore (riga * 1.0)
        
        drones[i].setPosition(targetX, targetY); // Imposta la posizione del drone calcolata sulla griglia
        drones[i].setSector(sectorIndex); // Assegna il drone nel settore calcolato
        sectorQueues[sectorIndex].push(i); // Aggiunge l'indice del drone alla coda del settore corrispondente
        
        sendingDataToClient(drones[i]); // Invio dei dati iniziali del drone al client
        readConfirmationFromClient(); // Attende una conferma dal client per l'inizializzazione
        
        // Se è il primo drone nel settore, lo avvia subito
        if (droneIndex == 0) {         
            drones[i].start(); // Solo il primo drone inizia immediatamente
        }

   }
   
}


// Invia i dati dei droni attivi al client
void ControlCenter::sendingDataToClient(Drone &drone) {
    // Dichiarazione di buffer per memorizzare le chiavi e i valori che saranno inviati a Redis
    char key1[50], value1[50];
    char key2[50], value2[50];
    char key3[50], value3[50];
    char key4[50], value4[50];
    char key5[50], value5[50];
    char key6[50], value6[50];

    // Incrementa il contatore di invio per tenere traccia del numero di messaggi inviati
    send_counter++;

    // Formattazione delle coppie chiave-valore
    sprintf(key1, "ID");
    sprintf(value1, "%d", drone.getId());

    sprintf(key2, "posX");
    sprintf(value2, "%f", drone.getX());

    sprintf(key3, "posY");
    sprintf(value3, "%f", drone.getY());

    sprintf(key4, "Sector");
    sprintf(value4, "%d", drone.getSector());

    sprintf(key5, "Distance");
    sprintf(value5, "%f", drone.getDistanceTraveled());

    sprintf(key6, "Battery");
    sprintf(value6, "%f", drone.getBatteryLife());

    // Invio dei valori formattati alla stream WRITE_STREAM
    reply = RedisCommand(c2r, "XADD %s * %s %s %s %s %s %s %s %s %s %s %s %s", 
                         WRITE_STREAM, 
                         key1, value1, 
                         key2, value2, 
                         key3, value3, 
                         key4, value4, 
                         key5, value5, 
                         key6, value6);

    // Verifica del tipo di risposta ottenuta
    assertReplyType(c2r, reply, REDIS_REPLY_STRING);

    // Stampa un messaggio di debug con i dettagli del messaggio inviato allo stream
    printf("main(): pid %d: user %s: stream %s: Date: %s -> %s, %s -> %s, %s -> %s, %s -> %s, %s -> %s, %s -> %s (id: %s)\n", 
           pid, username, WRITE_STREAM, 
           key1, value1, 
           key2, value2, 
           key3, value3, 
           key4, value4, 
           key5, value5, 
           key6, value6, 
           reply->str);

    // Liberazione della memoria per la risposta
    freeReplyObject(reply);
    
}


// Legge conferma dal client
void ControlCenter::readConfirmationFromClient() {
    
    char streamname[100]; // Nome dello stream Redis
    char msgid[100]; // ID del messaggio
    char fval[100]; // Valore del campo
    int i, k, h; // Variabili di loop
 

    // Incrementa il contatore delle letture dal client
    read_counter++;
    // Comando Redis per leggere il messaggio dal gruppo di stream
    reply = RedisCommand(c2r, "XREADGROUP GROUP diameter %s BLOCK %d COUNT 1 NOACK STREAMS %s >", username, block, READ_STREAM);
    // Stampa un messaggio di debug per indicare che il messaggio è stato letto
    printf("main(): pid %d: user %s: Read msg %d from stream %s\n", pid, username, read_counter, READ_STREAM );

    // Verifica se la risposta è valida
    assertReply(c2r, reply);
    dumpReply(reply, 0);

    // Elaborazione del messaggio ricevuto ciclando tutti gli stream letti
    for (k = 0; k < ReadNumStreams(reply); k++) {
        // Ottiene il nome dello stream e lo memorizza in 'streamname'
        ReadStreamName(reply, streamname, k);
        // Ciclo attraverso tutti i messaggi nello stream
        for (i = 0; i < ReadStreamNumMsg(reply, k); i++) {
            // Ottiene l'ID del messaggio e lo memorizza in 'msgid'
            ReadStreamNumMsgID(reply, k, i, msgid);
            // Stampa un messaggio di debug con i dettagli del messaggio letto
            printf("main(): pid %d: user %s: stream %s, streamnum %d, msg %d, msgid %s with %d values\n", pid, username, streamname, k, i, msgid, ReadStreamMsgNumVal(reply, k, i));

            // Ciclo attraverso tutti i valori del messaggio
            for (h = 0; h < ReadStreamMsgNumVal(reply, k, i); h++) {
                // Ottiene il valore h-esimo e lo memorizza in 'fval'
                ReadStreamMsgVal(reply, k, i, h, fval);
                // Stampa i dettagli del valore letto
                printf("main(): pid %d: user %s: streamnum %d, msg %d, msgid %s value %d = %s\n",
                       pid, username, k, i, msgid, h, fval);
            }
        }
    }

    // Rilascia la memoria della risposta
    freeReplyObject(reply);
    // il server "dorme" per un breve periodo di tempo
    server_micro_sleep(500000);
}


// Dorme per il numero di microsecodi richiesto
int ControlCenter::server_micro_sleep(long usec)
{
    struct timespec ts;
    int res;
    
    // Verifica se il numero di microsecondi è negativo
    if (usec < 0)
    {
        errno = EINVAL;
        return -1;
    }
     
    // Calcola il numero di secondi e nanosecondi
    ts.tv_sec = usec / 1000000;      // Converte i microsecondi in secondi
    ts.tv_nsec = (usec % 1000000) * 1000; // Restanti microsecondi convertiti in nanosecondi
    
    // Ciclo per gestire eventuali interruzioni di 'nanosleep'
    do {
        res = nanosleep(&ts, &ts);  // Tenta di dormire per il tempo specificato, aggiornando 'ts' in caso di interruzione
    } while (res && errno == EINTR); // Ripete se l'interruzione è causata da un segnale 

    
    return res; // Ritorna 0 se 'nanosleep' ha avuto successo, o un valore negativo in caso di errore

}


// Aggiorna lo stato dei droni
void ControlCenter::updateDrones() {
    
    // Ciclo per aggiornare ogni drone attivo
    for (auto &drone : drones) {
       // Verifica se il drone è attivo
       if(drone.isActive()){
          // Controlla se il livello della batteria è sopra la soglia di sicurezza
          if(drone.getBatteryLife() > batteryThreshold){
              // Se la batteria è  sufficiente, aggiorna la posizione del drone
              updatePosition(drone);
          }
          else{
              // Se la batteria è sotto la soglia, manda il drone alla base
              returnToBase(drone);
          }
       }
    }
    // Ciclo per aggiornare lo stato dei droni in ricarica
    for(auto it = chargingDrones.begin(); it != chargingDrones.end(); ) {
        // Esegue la ricarica del drone
        recharge(*it);
        // Verifica se il drone ha completato la ricarica
        if(!it->isRecharging()){
             // Se la ricarica è completa, sposta il drone nel vettore dei droni di riserva
             reserveDrones.push_back(*it);
             // Rimuove il drone dal vettore dei droni in ricarica e aggiorna l'iteratore
             it = chargingDrones.erase(it);
        }
        else{
            // Se la ricarica non è completa, passa al prossimo drone
            ++it;
        }
    }
    // Gestisce la rotazione e la gestione delle code dei droni nei vari settori
    manageDroneQueues();      
}


// Aggiorna la posizione del drone
void ControlCenter::updatePosition(Drone &drone) {

    if (drone.isRecharging() || !drone.isActive()) return; // Se il drone è in ricarica o non è attivo, non aggiornare la posizione
    
    double step = 0.25; // Aggiorna la posizione del drone di 250 metri
    drone.increaseDistanceTraveled(step); // Aggiunge la distanza percorsa alla distanza totale
    drone.increaseDistanceSector(step); // Aggiunge la distanza percorsa alla distanza nel settore

    // Calcola la posizione iniziale del settore
    int sectorX = drone.getSector() % gridSize; // Calcola la coordinata X del settore
    int sectorY = drone.getSector() / gridSize; // Calcola la coordinata Y del settore
    double startX = sectorX; // Posizione iniziale X del settore (sectorX * 1.0)
    double startY = sectorY; // Posizione iniziale Y del settore (sectorY * 1.0)

    // Calcola la posizione relativa all'interno del settore
    double relX = drone.getX() - startX;
    double relY = drone.getY() - startY;

    // Movimento a zig-zag in incrementi di 250 metri
    if (drone.getDistanceSector() <= 6.0) { // Fase di andata (6 km salita)
        if (int(relY / step) % 2 == 0) { // Riga pari, movimento a destra
            relX += step;
             // Se X supera il limite del settore, lo imposta a 1.0 e incrementa Y
            if (relX > 1.0) {
                relX = 1.0;
                relY += step;
            }
        } 
        else { // Riga dispari, movimento a sinistra
            relX -= step;
            // Se X scende sotto il limite del settore, lo imposta a 0.0 e incrementa Y
            if (relX < 0.0) {
                relX = 0.0;
                relY += step;
            }
        }
    } 
    else if (drone.getDistanceSector() <= 6.25) { // Passaggio da fase di andata a fase di ritorno
           relX = 1.0;
           relY -= step;
    
    }   
    
    else {  // Fase di ritorno (3,75 km discesa)
        if (int(relY / step) % 2 == 0) { // Riga pari, movimento a destra
            relX += step;
            // Se X supera il limite del settore, lo imposta a 1.0 e decrementa Y
            if (relX > 1.0) {
                relY -= step;
                relX = 1.0;
            }
        } else { // Riga dispari, movimento a sinistra
            relX -= step;
            // Se X scende sotto il limite del settore, lo imposta a 0.0 e decrementa Y
            if (relX < 0.0) {
                relY -= step;
                relX = 0.0;
            }
        }
    }
    
    // Se la distanza nel settore è di 10 km, resetta il contatore
    if(drone.getDistanceSector() == 10.0) {
            drone.resetDistanceSector();
    }

    // Consumo della batteria in base alla distanza percorsa
    drone.consumeBattery(drone.getBatteryConsumptionPerMeter() * 250);

    // Aggiorna la posizione del drone
    drone.setPosition(startX + relX, startY + relY);
    
    // Invia i dati aggiornati al client e attende conferma
    sendingDataToClient(drone);
    readConfirmationFromClient();
    
}


// Ritorno del drone alla base
void ControlCenter::returnToBase(Drone &drone) {

    double deltaX = baseX - drone.getX(); // Calcola la differenza X tra il drone e la base
    double deltaY = baseY - drone.getY(); // Calcola la differenza Y tra il drone e la base
    double distance = sqrt(deltaX * deltaX + deltaY * deltaY); // Calcola la distanza tra la posizione corrente del drone e la base
    drone.increaseDistanceTraveled(distance); // Aggiorna la distanza percorsa in totale con la distanza per tornare al centro di controllo    
    
    // Consumo della batteria in base alla distanza percorsa per tornare alla base
    drone.consumeBattery((drone.getBatteryConsumptionPerMeter() * 1000) * distance );
    
    int sector_assignment = drone.getSector(); // Salvo il settore del drone per la sostituzione 
    
    drone.setPosition(baseX, baseY); // Imposta la posizione del drone alla base    
    drone.resetDistanceTraveled(); // Rispristino la distanza totale percorsa
    drone.resetDistanceSector(); // Ripristino la distanza settoriale percorsa
    drone.resetSector(); // Ripristino il settore
    drone.setActive(false); // Disattiva il drone, poichè ora è alla base e non è più operativo
    
    drone.setMaxRechargeTime(getRandomRechargeTime()); // Imposta il tempo massimo di ricarica per il drone
    // Calcola il tempo necessario per ricaricare la batteria in base al livello attuale della batteria
    double rechargeTime = ((drone.getMaxRechargeTime() * ((100 - drone.getBatteryLife()) / 100.0))+1); 
    setRechargeTime(drone, rechargeTime); // Imposta il tempo di ricarica calcolato
 

    chargingDrones.push_back(drone); // Inserisce il drone nel vettore dei droni in ricarica
    replaceDrone(drone, sector_assignment); // Sostituisce il drone con uno di riserva
    sendingDataToClient(drone); // Invia i dati aggiornati del nuovo drone attivo al client
    readConfirmationFromClient(); // Legge la conferma del client per la ricezione dei dati
    
    
}


// Restituisce un tempo di ricarica casuale
double ControlCenter::getRandomRechargeTime() const {
    return 120.0 + static_cast<double>(rand() % 61); // Tra 120 e 180 minuti
}


// Imposta il tempo di ricarica del drone
void ControlCenter::setRechargeTime(Drone &drone, double rechargeTime) {
    drone.setRechargeTime(rechargeTime); // Imposta il tempo di ricarica rimanente
    drone.setRecharging(true); // Imposta lo stato di ricarica
}


// Sostituisce un drone con uno di riserva
void ControlCenter::replaceDrone(Drone &drone, int sector_assignment) {

    int index = -1; // Variabile index che conterrà l'indice del drone da sostituire
    // Cera l'indice del drone con l'ID uguale a quello passato in input
    for (size_t i = 0; i < drones.size(); ++i) {
         if(drones[i].getId() == drone.getId()) {
             index = i;
             break; // Termina il ciclo una volta trovato il drone
         }
    }
    
  
    // Inizializza un puntatore al drone attivo con la maggiore distanza percorsa nello stesso settore
    Drone *prevDrone = nullptr;
    // Cicla attrverso tutti i droni per trovare il drone attivo con la maggiore distanza percorsa nel settore
    for (auto &d : drones) {
         if(d.isActive() && !d.isRecharging() && d.getSector() == sector_assignment && d.getId() != drone.getId()) {
              // Se prevDrone non è stato ancora trovato o se l'attuale drone ha percorso più distanza
              if(!prevDrone || d.getDistanceTraveled() > prevDrone->getDistanceTraveled()) {
              
                  prevDrone = &d; // Aggiorna prevDrone con il drone che ha percorso la maggiore distanza
              }
         }
    }
    // Inizializza la nuova posizione di partenza del drone di riserva con la posizione del centro di controllo
    double newStartX = baseX, newStartY = baseY;
    // Se esiste un drone attivo precedente nello stesso settore
    if(prevDrone) {
         // Ottiene la posizione del drone con la maggiore distanza percorsa
         double prevX = prevDrone->getX();
         double prevY = prevDrone->getY();
         // Distanza che il nuovo drone dovrà coprire
         double distanceToCover = 2.75;
         // Ottiene la distanza percorsa dal drone nel settore
         double distanceSectorMove = prevDrone->getDistanceSector();
         // Calcola la nuova posizione del drone di riserva nel settore in base alla distanza coperta dal drone precedente
         std::tie(newStartX, newStartY) = calculateNewPositionInSector(prevX, prevY, distanceToCover, sector_assignment, distanceSectorMove);
    }
    // Se c'è almeno un drone di riserva disponibile
    if(!reserveDrones.empty()) {
        Drone reserveDrone = reserveDrones.front(); // Prendi il primo drone dal vettore di riserva
        
        reserveDrone.setPosition(newStartX, newStartY); // Imposta la nuova posizione
        reserveDrone.setSector(sector_assignment); // Imposta il nuovo settore
        reserveDrone.setActive(true); // Attiva il drone di riserva
        
        drones[index] = reserveDrone; // Sostituisci il drone esaurito con il drone di riserva nel vettore drones
        reserveDrones.erase(reserveDrones.begin()); // Rimuovi il drone appena utilizzato dal vettore dei droni di riserva
         
    }
}


// Calcola la nuova posizione che deve assumere il nuovo drone attivo
std::pair<double,double> ControlCenter::calculateNewPositionInSector(double prevX, double prevY, double distance, int sector, double distanceSectorMove) {
 
    // Calcola la posizione iniziale del settore in base alla griglia
    int sectorX = sector % gridSize; // Ottiene la coordinata X del settore nella griglia (colonna)
    int sectorY = sector / gridSize; // Ottiene la coordinata Y del settore nella griglia (riga)
    double startX = sectorX; // La posizione iniziale X del settore (sectorX * 1.0)
    double startY = sectorY; // La posizione iniziale Y del settore (sectorY * 1.0)
    
    // Calcola la posizione relativa all'interno del settore
    double relX = prevX - startX; // Posizione relativa X rispetto all'inizio del settore
    double relY = prevY - startY; // Posizione relativa Y rispetto all'inizio del settore
    double newX = relX;  // Imposta la nuova coordinata X
    double newY = relY;  // Imposta la nuova coordinata Y
    
    // Inizializza le variabili per la distanza percorsa e il passo del drone
    double distanceCovered = 0.0;
    double step = 0.25;
   
    // Inizia il ciclo per calcolare la nuova posizione partendo dalle coordinate del drone con distanza maggiore e percorrendo 2.75 km 
    while(distanceCovered < distance){ // Continua finchè la distanza coperta non raggiunge almeno 2,75 km    
           // Se il drone ha già percorso 10 km, resetta il contatore di distanza settoriale
           if(distanceSectorMove == 10.0) {
                     distanceSectorMove = 0.0;
            }
           // Fase di andata del drone (fino a 6 km in salita)
           if(distanceSectorMove <= 6.0) {
          
                  if(int(newY / step) % 2 == 0) { // Riga pari, movimento a destra
                     newX += step;
                     distanceCovered += step;
                     distanceSectorMove += step;
                     // Se X supera il limite del settore, lo imposta a 1.0 e incrementa Y
                     if(newX > 1.0) {
                        newX = 1.0;
                        newY += step;
                  
                     }
                  }
                  else{ // Riga dispari, movimento a sinistra
                     newX -= step;
                     distanceCovered += step;
                     distanceSectorMove += step;
                     // Se X scende sotto il limite del settore, lo imposta a 0.0 e incrementa Y
                     if(newX < 0.0) {
                        newX = 0.0;
                        newY += step;
                  
                    }
                 }
           }
          // Passaggio da fase di andata a fase di ritorno
          else if( distanceSectorMove <= 6.25){
             newX = 1.0;
             newY -= step;
             distanceCovered += step;
             distanceSectorMove += step;
       
          }
  
          else{ // Fase di ritorno del drone (3,75 km in discesa)

               if(int(newY / step) % 2 == 0) { // Riga pari, movimento a destra
                   newX += step;
                   distanceCovered += step;
                   distanceSectorMove += step;
                   // Se X supera il limite del settore, lo imposta a 1.0 e decrementa Y
                   if(newX > 1.0 ) {
                     newX = 1.0;
                     newY -= step;
                  }
               }
               else{ // Riga dispari, movimento a sinistra
                  newX -= step;
                  distanceCovered += step;
                  distanceSectorMove += step;
                  // Se X scende sotto il limite del settore, lo imposta a 0.0 e decrementa Y
                  if(newX < 0.0) {
                     newX = 0.0;
                     newY -= step;
                  
                  }
               }
            }
    
        }
     // Aggiusta le nuove coordinate in base alla posizione di partenza del settore   
     newX += startX;
     newY += startY;
     // Restituisce la nuova posizione come coppia di coordiante
     return {newX, newY};
}


// Esegue la ricarica del drone
void ControlCenter::recharge(Drone &drone) {
    if (!drone.isRecharging()) return; // Se il drone non è in ricarica, esce dalla funzione
    // Variabile del decremento del tempo in minuti 
    double minutes = 1.0;
    // Decrementa il tempo di ricarica rimanente
    drone.setRechargeTime(max(drone.getRechargeTime() - minutes, 0.0)); // Simula il passaggio del tempo di ricarica
    // Controlla se il tempo di ricarica è esattamente 0
    if (drone.getRechargeTime() == 0) {
        drone.setBatteryLife(100.0); // Ripristina la batteria al massimo
        drone.setRecharging(false); // Termina la ricarica
 
    }
}


// Gestisce le code dei droni nei settori
void ControlCenter::manageDroneQueues() {
    // Ciclo attraverso ogni settore
    for (int i = 0; i < numSectors; ++i) {
        // Ottiene la coda di droni associata al settore corrente
        auto &queue = sectorQueues[i];
        // Se la coda non è vuota 
        if (!queue.empty()) {
            // Ottiene l'ID del drone in testa alla coda
            int droneId = queue.front();
            // Ottiene una referenza al drone corrispondente nel vettore dei droni attivi usando l'ID
            Drone &drone = drones[droneId];
            if (drone.getDistanceTraveled() >= 2.5) { // Se il drone ha percorso almeno 2,5 km
                queue.pop(); // Rimuove il drone attuale dalla coda
                if (!queue.empty()) { // Se ci sono ancora droni nella coda
                    int nextDroneId = queue.front(); // Prende l'ID del prossimo drone in coda
                    drones[nextDroneId].start(); // Inizia il movimento del prossimo drone
                }
            }
        }
    }
}


// Visualizza le specifiche di ogni drone 
void ControlCenter::printDroneStatus() const {
    
    // Stampa lo stato dei droni attivi
    printf("\n== STATO DEI DRONI ATTIVI ==\n");
    for (const auto &drone : drones) {
      printf("Drone ID: %d Settore: %d Posizione: (%.2f, %.2f) Distanza percorsa: %.2f Stato batteria: %.2f In ricarica: %s\n", 
      drone.getId(), drone.getSector(), drone.getX(), drone.getY(), drone.getDistanceTraveled(), drone.getBatteryLife(), drone.isRecharging() ? "Si" : "No");
    }
     
    // Stampa lo stato dei droni in ricarica
    printf("\n== STATO DEI DRONI IN RICARICA ==\n");
    for (const auto &drone : chargingDrones) {
      printf("Drone ID: %d Stato batteria: %.2f Tempo di ricarica max: %.2f Tempo rimanente per la ricarica: %.2f In ricarica: %s\n", drone.getId(), drone.getBatteryLife(), drone.getMaxRechargeTime(), drone.getRechargeTime(), drone.isRecharging() ? "Si" : "No"); 
    }
     
    // Stampa lo stato dei droni di riserva
    printf("\n== STATO DEI DRONI DI RISERVA ==\n");
    for(const auto &drone : reserveDrones) {
      printf("Drone ID: %d Settore: %d Posizione: (%.2f, %.2f) Distanza percorsa: %.2f Stato batteria: %.2f In ricarica: %s\n", 
      drone.getId(), drone.getSector(), drone.getX(), drone.getY(), drone.getDistanceTraveled(), drone.getBatteryLife(), drone.isRecharging() ? "Si" : "No");
    }
}


// Registra i log nel database
void ControlCenter::log2DB(Con2DB db) {

    PGresult *res; // Dichiarazione di un puntatore per contenere il risultato dell'esecuzione dei comandi SQL
    char sqlcmd[1024]; // Buffer per memorizzare i comandi SQL

    // Inizio della transazione
    sprintf(sqlcmd, "BEGIN"); // Prepara il comnado SQL per avviare una transazione
    res = db.ExecSQLcmd(sqlcmd); // Esegue il comando SQL
    PQclear(res); // Libera la memoria allocata per il risultato
    
   
    // Inserimento log per ogni drone attivo
    for (const auto& drone : drones) {
        sprintf(sqlcmd,
                "INSERT INTO DroneLog (id, pos_x, pos_y, sector, distance, battery, rechargetime, pid)" 
                "VALUES (%d, %f, %f, %d, %f, %f, %f, %d) "
                "ON CONFLICT (id) DO UPDATE SET " // Se esiste già un record con lo stesso id, aggiorna i dati
                "pos_x = EXCLUDED.pos_x, "
                "pos_y = EXCLUDED.pos_y, "
                "sector = EXCLUDED.sector, "
                "distance = EXCLUDED.distance, "
                "battery = EXCLUDED.battery, "
                "rechargetime = EXCLUDED.rechargetime, "
                "pid = EXCLUDED.pid, "
                "time = CURRENT_TIMESTAMP;",
                drone.getId(), drone.getX(), drone.getY(), drone.getSector(),
                drone.getDistanceTraveled(), drone.getBatteryLife(), drone.getRechargeTime(), pid);
        res = db.ExecSQLcmd(sqlcmd); // Esegue il comando SQL
        PQclear(res); // Libera la memoria allocata per il risultato
      
    }
    
   
    // Inserimento log per ogni drone in ricarica
    for (const auto& drone : chargingDrones) {

        sprintf(sqlcmd,
                "INSERT INTO DroneLog (id, pos_x, pos_y, sector, distance, battery, rechargetime, pid)"
                "VALUES (%d, %f, %f, %d, %f, %f, %f, %d) "
                "ON CONFLICT (id) DO UPDATE SET "
                "pos_x = EXCLUDED.pos_x, "
                "pos_y = EXCLUDED.pos_y, "
                "sector = EXCLUDED.sector, "
                "distance = EXCLUDED.distance, "
                "battery = EXCLUDED.battery, "
                "rechargetime = EXCLUDED.rechargetime, "
                "pid = EXCLUDED.pid, "
                "time = CURRENT_TIMESTAMP;",
                drone.getId(), drone.getX(), drone.getY(), drone.getSector(),
                drone.getDistanceTraveled(), drone.getBatteryLife(), drone.getRechargeTime(), pid);
        res = db.ExecSQLcmd(sqlcmd); // Esegue il comando SQL
        PQclear(res); // Libera la memoria allocata per il risultato
       
    
    }
    
   
    // Inserimento log per ogni drone di riserva
    for (const auto& drone : reserveDrones) {
    
        sprintf(sqlcmd,
                "INSERT INTO DroneLog (id, pos_x, pos_y, sector, distance, battery, rechargetime, pid)"
                "VALUES (%d, %f, %f, %d, %f, %f, %f, %d) "
                "ON CONFLICT (id) DO UPDATE SET "
                "pos_x = EXCLUDED.pos_x, "
                "pos_y = EXCLUDED.pos_y, "
                "sector = EXCLUDED.sector, "
                "distance = EXCLUDED.distance, "
                "battery = EXCLUDED.battery, "
                "rechargetime = EXCLUDED.rechargetime, "
                "pid = EXCLUDED.pid, "
                "time = CURRENT_TIMESTAMP;",
                drone.getId(), drone.getX(), drone.getY(), drone.getSector(),
                drone.getDistanceTraveled(), drone.getBatteryLife(), drone.getRechargeTime(),pid);
        res = db.ExecSQLcmd(sqlcmd); // Esegue il comando SQL
        PQclear(res); // Libera la memoria associata al risultato della query
    
    
    }

    // Conferma della transazione
    sprintf(sqlcmd, "COMMIT"); // Prepara il comando SQL per confermare la transazione
    res = db.ExecSQLcmd(sqlcmd); // Esegue il comando SQL
    PQclear(res); // Libera la memoria associata al risultato della query
}


// Termina connessione con Redis
void ControlCenter::finishConnectionRedis() {
    redisFree(c2r);
}




