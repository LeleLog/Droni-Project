#include "main.h"
// Costruttore che inizializza i membri della classe con valori predefiniti
Time::Time() {

   time_step = 0; // Inizializza il contatore di passi temporali
   nanos = 0; // Inizializza il conteggio dei nanosecondi
   last_nanos = get_nanos(); // Inizializza i nanosecondi dell'ultima iterazione con i nanosecondi attuali
   nanos_day = 0; // Inizializza i nanosecondi del giorno corrente
   global_time_sec = 0.0; // Inizializza il tempo globale in secondi
   timeadvance = 0.0; // Inizializza il tempo avanzato tra due passi temporali
}
// Restituisce il valore del contatore di passi temporali
int Time::getTime_Step() const {
    return time_step;
}
// Restituisce il valore dei nanosecondi correnti
long int Time::getNanos() const {
    return nanos;
}
// Restituisce il valore dei nanosecondi dell'ultima iterazione
long int Time::getLastNanos() const {
    return last_nanos;
}
// Restituisce il valore dei nanosecondi relativi al giorno corrente
long int Time::getNanosDay() const {
    return nanos_day;
}
// Restituisce il valore del tempo globale in secondi
double Time::getGlobalTime() const {
    return global_time_sec;
}
// Restituisce il tempo avanzato tra due passi temporali
double Time::getTimeAdvance() const {
    return timeadvance;
}
// Aggiorna i valori temporali basati sui nanosecondi correnti
void Time::update_time() {
   
   nanos = get_nanos(); // Ottiene i nanosecondi attuali
   // Calcola il tempo avanzato in secondi
   timeadvance = ((double) (nanos - last_nanos))/((double) (1000000000L));
   // Aggiorna il tempo globale aggiungendo il tempo avanzato
   global_time_sec = global_time_sec + timeadvance;
    
   last_nanos = nanos; // Aggiorna i nanosecondi dell'ultimo aggiornamento
   nanos_day = get_day_nanos(buf); // Ottiene i nanosecondi relativi al giorno corrente e li salva in nanos_day
   time_step++; // Incrementa il contatore dei passi temporali

}
// Stampa lo stato corrente del programma temporale, inclusi tutti i valori temporali
void Time::print_time_program() {
 
   printf("\nTime step: %d Global time: %.6f Iteration time: %.6f Nanos: %ld Time Corrent Data: %s Nanos Day: %ld\n", time_step, global_time_sec, timeadvance, nanos, buf, nanos_day);
  

}
// Restituisce il tempo attuale in nanosecondi rispetto all'epoca Unix
int long Time::get_nanos(void) const {
    struct timespec ts;
    timespec_get(&ts, TIME_UTC); // Ottiene il tempo corrente in formato timespec
    return ((long) (ts.tv_sec * 1000000000L + ts.tv_nsec)); // Converte i secondi e nanosecondi in nanosecondi totali
}

// Calcola e restituisce i nanosecondi relativi al giorno corrente
long int Time::get_day_nanos(char* buf) const {
   struct tm *info; // Struttura pe memorizzare i dati temporali (anno, mese, giorno)
   struct timespec ts;

   timespec_get(&ts, TIME_UTC); // Ottiene il tempo attuale in formato timespec
    
   info = localtime( &(ts.tv_sec) ); // Converte i secondi in una struttura temporale leggibile
   //Formatta il tempo corrente in una stringa (buf) con il formato standard SQL ISO 8601 1997-12-17 07:37:16-08
   sprintf(buf, "%.4d-%.2d-%.2d %.2d:%.2d:%.2d",
	   info -> tm_year + 1900, // Anno
	   info -> tm_mon + 1, // Mese
	   info -> tm_mday, // Giorno
	   info -> tm_hour, // Ora
	   info -> tm_min, // Minuti
	   info -> tm_sec // Secondi
	   );

   return (ts.tv_nsec); // Restituisce i nanosecondi correnti del giorno
}








