#include "main.h"


// Implementazione del costruttore della classe Drone
Drone::Drone(int id, double startX, double startY, double batteryLife)
    : id(id), x(startX), y(startY), batteryLife(batteryLife), speed(30.0), autonomy(30.0), recharging(false), rechargeTimeRemaining(0), sector(-1), active(false), distanceTraveled(0), distanceSector(0), maxRechargeTime(0){
    
    maxDistance = speed * (autonomy / 60.0); // Distanza massima percorribile in km (15 km)
    batteryConsumptionPerMeter = 100.0 / (maxDistance * 1000); // Percentuale di batteria consumata per metro percorso
    
}

// Restituisce l'ID del drone
int Drone::getId() const {
    return id;
}

// Restituisce la posizione X del drone
double Drone::getX() const {
    return x;
}

// Restituisce la posizione Y del drone
double Drone::getY() const {
    return y;
}

// Imposta la posizione del drone a una nuova coppia di coordinate (x,y)
void Drone::setPosition(double newX, double newY) {
    x = newX;
    y = newY;
}

// Restituisce lo stato della batteria del drone in percentuale
double Drone::getBatteryLife() const {
    return batteryLife;
}

// Restituisce l'autonomia del drone in minuti
double Drone::getAutonomy() const {
    return autonomy;
}

// Imposta il settore di operazione del drone
void Drone::setSector(int sector) {
    this->sector = sector;
}

// Restituisce il settore assegnato al drone
int Drone::getSector() const {
    return sector;
}

// Avvia il movimento del drone
void Drone::start() {
    active = true;
}

// Verifica se il drone è in fase di ricarica
bool Drone::isRecharging() const {
    return recharging;
}

// Verifica se il drone è attivo
bool Drone::isActive() const {
    return active;
}

// Imposta lo stato di ricarica del drone (vero/falso)
void Drone::setRecharging(bool recharging) {
    this->recharging = recharging;
}

// Restituisce la distanza totale percorsa dal drone
double Drone::getDistanceTraveled() const {
    return distanceTraveled;
}

// Incrementa la distanza totale percorsa dal drone
void Drone::increaseDistanceTraveled(double distance) {
    distanceTraveled += distance;
}

// Consumo della batteria in base al valore di ingresso
void Drone::consumeBattery(double consumption) {
    batteryLife -= consumption;
}

// Imposta il livello della batteria
void Drone::setBatteryLife(double life) {
    batteryLife = life;
}

// Imposta il tempo di ricarica del drone
void Drone::setRechargeTime(double rechargeTime) {
    rechargeTimeRemaining = rechargeTime;
}

// Restituisce il tempo di ricarica del drone
double Drone::getRechargeTime() const {
    return rechargeTimeRemaining;
}

// Imposta il settore del drone a uno stato non assegnato
void Drone::resetSector() {
    sector = -1;
}

// Restituisce la velocità del drone
double Drone::getSpeed() const {
    return speed;
}

// Restituisce il consumo della batteria per ogni metro percorso
double Drone::getBatteryConsumptionPerMeter() const {
    return batteryConsumptionPerMeter;
}

// Imposta lo stato attivo del drone
void Drone::setActive(bool active) {
    this->active = active;
}

// Restituisce la distanza percorsa nel settore assegnato
double Drone::getDistanceSector() const {
    return distanceSector;
}

// Incrementa la distanza percorsa nel settore
void Drone::increaseDistanceSector(double distance) {
    distanceSector += distance;
}

// Ripristina la distanza percorsa nel settore a zero
void Drone::resetDistanceSector() {
    distanceSector = 0.0 ;
}

// Ripristina la distanza totale percorsa a zero
void Drone::resetDistanceTraveled() {
    distanceTraveled = 0.0 ;
}

// Restituisce il tempo massimo di ricarica compreso tra i 120-180 minuti
double Drone::getMaxRechargeTime() const {
    return maxRechargeTime;
}

// Imposta il tempo massimo di ricarica
void Drone::setMaxRechargeTime(double maxRechargeTime) {
    this->maxRechargeTime = maxRechargeTime;
}









