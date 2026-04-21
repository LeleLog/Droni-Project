#ifndef DRONE_H
#define DRONE_H

#include "main.h"

class Drone {
public:

    // Costruttore della classe Drone che inizializza il drone con i seguenti parametri
    Drone(int id, double startX, double startY, double batteryLife); //

    // Restituisce l'ID del drone
    int getId() const; //

    // Restituisce la posizione X del drone
    double getX() const; //

    // Restituisce la posizione Y del drone
    double getY() const; //

    // Imposta la posizione del drone a una nuova coppia di coordinate (x,y)
    void setPosition(double newX, double newY); //

    // Restituisce lo stato della batteria del drone in percentuale
    double getBatteryLife() const; //

    // Imposta il settore di operazione del drone
    void setSector(int sector); //

    // Restituisce il settore assegnato al drone
    int getSector() const; //
    
    // Restituisce l'autonomia del drone in minuti
    double getAutonomy() const; 

    // Avvia il movimento del drone
    void start(); //

    // Verifica se il drone è in fase di ricarica
    bool isRecharging() const; //

    // Verifica se il drone è attivo
    bool isActive() const; //

    // Imposta lo stato di ricarica del drone (vero/falso)
    void setRecharging(bool recharging); //

    // Restituisce la distanza totale percorsa dal drone
    double getDistanceTraveled() const; //

    // Incrementa la distanza totale percorsa dal drone
    void increaseDistanceTraveled(double distance); //

    // Consumo della batteria in base al valore di ingresso
    void consumeBattery(double consumption); //

    // Imposta il livello della batteria
    void setBatteryLife(double life); //

    // Imposta il tempo di ricarica del drone
    void setRechargeTime(double rechargeTime); //

    // Restituisce il tempo di ricarica del drone
    double getRechargeTime() const; //

    // Imposta il settore del drone a uno stato non assegnato
    void resetSector();

    // Restituisce la velocità del drone
    double getSpeed() const;

    // Restituisce il consumo della batteria per ogni metro percorso
    double getBatteryConsumptionPerMeter() const; //
    
    // Imposta lo stato attivo del drone
    void setActive(bool active); //
    
    // Restituisce la distanza percorsa nel settore assegnato
    double getDistanceSector() const; //
    
    // Incrementa la distanza percorsa nel settore
    void increaseDistanceSector(double distance); //
    
    // Ripristina la distanza percorsa nel settore a zero
    void resetDistanceSector();  //
    
    // Ripristina la distanza totale percorsa a zero
    void resetDistanceTraveled(); 
    
    // Restituisce il tempo massimo di ricarica compreso tra i 120-180 minuti
    double getMaxRechargeTime() const; //
    
    // Imposta il tempo massimo di ricarica
    void setMaxRechargeTime(double maxRechargeTime); //
    

    
private:
    int id; // ID univoco del drone
    double x, y; // Coordinate di posizione del drone
    double batteryLife; // Batteria del drone in percentuale
    double speed; // Velocità del drone
    double autonomy; // Autonomia operativa in minuti
    bool recharging; // Stato di ricarica del drone
    double rechargeTimeRemaining; // Tempo rimanente per completare la ricarica
    int sector; // Settore operativo del drone
    bool active; // Stato attivo del drone
    double distanceTraveled; // Distanza totale percorsa dal drone
    double distanceSector; // Distanza percorsa nel settore attuale
    double maxDistance; // Distanza massima percorribile con una carica completa
    double batteryConsumptionPerMeter; // Percentuale di batteria consumata per metro percorso
    double maxRechargeTime; // Tempo massimo necessario per una ricarica completa
   
    
};

#endif // DRONE_H
