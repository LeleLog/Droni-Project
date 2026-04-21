#ifndef TIMEPROGRAM_H
#define TIMEPROGRAM_H

#include <main.h>

class Time {
public:

   // Costruttore della classe, che inizializza le variabili interne 
   Time();
   // Restituisce il valore del contatore di passi temporali
   int getTime_Step() const;
   // Restituisce il valore dei nanosecondi correnti
   long int getNanos() const;
   // Restituisce il valore dei nanosecondi dell'ultima iterazione
   long int getLastNanos() const;
   // Restituisce il valore dei nanosecondi relativi al giorno corrente
   long int getNanosDay() const;
   // Restituisce il valore del tempo globale in secondi
   double getGlobalTime() const;
   // Restituisce il tempo avanzato tra due passi temporali
   double getTimeAdvance() const;
   // Aggiorna i valori temporali basati sui nanosecondi correnti
   void update_time();
   // Stampa lo stato corrente del programma temporale, inclusi tutti i valori temporali
   void print_time_program();
   // Restituisce il tempo attuale in nanosecondi rispetto all'epoca Unix
   long int get_nanos(void) const;
   // Calcola e restituisce i nanosecondi relativi al giorno corrente
   long int get_day_nanos(char* buf) const;
   
private:
   int time_step; // Conta il numero di iterazioni o passi temporali eseguiti
   long int nanos; // Memorizza il tempo corrente in nanosecondi
   long int last_nanos; // Memorizza il tempo in nanosecondi dell'ultima iterazione
   long int  nanos_day; // Memorizza il numero di nanosecondi trascorsi dall'inizio del giorno corrente
   double global_time_sec; // Il tempo globale simulato in secondi dall'inizio del programma
   double timeadvance; // Indica il tempo trascorso tra due iterazioni in secondi
   char buf[200]; // Buffer utilizzato per memorizzare la data e l'ora attuali in formato stringa
};
#endif// TIME_H
