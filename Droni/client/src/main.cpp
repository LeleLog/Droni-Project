#include "main.h"

// Definisce i nomi degli stream di lettura e scrittura per Redis
#define READ_STREAM "stream2"
#define WRITE_STREAM "stream1"

using namespace std;


int main() {
  
    // Dichiarazione dei contesti e delle risposte di Redis
    redisContext *c2r;
    redisReply *reply;
    
    // Variabili contatori di messaggi letti e inviati
    int read_counter = 0;
    int send_counter = 0;
    int block = 1000000000; // Timeout per il blocco della lettura in microsecondi
    int pid; // ID del processo
    unsigned int seed; // Seed per la generazione casuale
    char username[100]; // Buffer per il nome utente
    char key[100]; // Buffer per il campo chiave
    char value[100]; // Buffer per il campo valore della chiave
    char streamname[100]; // Nome dello stream Redis
    char msgid[100]; // ID del messaggio
    char fval[100]; // Valore del campo
    int i, k, h; // Variabili di loop
    

#if (DEBUG > 0)
  setvbuf(stdout, (char*) NULL, _IONBF, 0);
  setvbuf(stderr, (char*) NULL, _IONBF, 0);
#endif
  
  // Inizializza il seed per la generazione di numeri casuali
  seed = (unsigned) time(NULL);
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


 // Loop principale per la comunicazione con Redis
 while (1)
   {
     // Lettura del messaggio inviato dal server
    read_counter++;
    reply = RedisCommand(c2r, "XREADGROUP GROUP diameter %s BLOCK %d COUNT 1 NOACK STREAMS %s >", username, block, READ_STREAM);
    
    // Stampa di informazioni sulla lettura del messaggio
    printf("main(): pid %d: user %s: Read msg %d from stream %s\n", pid, username, read_counter, READ_STREAM);

    // Verifica se la risposta è valida
    assertReply(c2r, reply);
    dumpReply(reply, 0);

    // Elaborazione del messaggio ricevuto
    bool confirmation = true; // Imposta la conferma a true inizialmente
    
    for (k = 0; k < ReadNumStreams(reply); k++) {
        // Legge il nome dello stream
        ReadStreamName(reply, streamname, k);
        
        for (i = 0; i < ReadStreamNumMsg(reply, k); i++) {
            // Legge l'ID del messaggio
            ReadStreamNumMsgID(reply, k, i, msgid);
            printf("main(): pid %d: user %s: stream %s, streamnum %d, msg %d, msgid %s with %d values\n",
                   pid, username, streamname, k, i, msgid, ReadStreamMsgNumVal(reply, k, i));

            // Stampa i valori ricevuti dal messaggio
            for (h = 1; h < ReadStreamMsgNumVal(reply, k, i); h = h + 2) {
                // Legge il valore di un campo e lo copia in fval
                ReadStreamMsgVal(reply, k, i, h, fval);
                printf("main(): pid %d: user %s: streamnum %d, msg %d, msgid %s value %d = %s\n",
                       pid, username, k, i, msgid, h, fval);
                       
                // Verifica se il valore del campo è nill o stringa vuota
                if( fval == nullptr || fval[0] == '\0'){
                
                        confirmation = false; // Imposta confirmation a false se c'è almeno un valore nill
                }
               
            }
        }
    }

     // Stampa finale per confermare il campo confirmation
     printf("main(): pid %d: user %s: confirmation = %d\n", pid, username, confirmation);

     // Rilascia la memoria della risposta
     freeReplyObject(reply);

     // Invio della conferma al server
     send_counter++;
     sprintf(key, "confirmation");
     sprintf(value, "%d", confirmation); // Converte confirmation in stringa
     
     reply = RedisCommand(c2r, "XADD %s * %s %s", WRITE_STREAM, key, value); // Aggiunge la conferma allo stream di scrittura
     assertReplyType(c2r, reply, REDIS_REPLY_STRING); // Verifica il tipo di risposta
     printf("main(): pid =%d: stream %s: %s:%s then send to server (id: %s)\n", pid, WRITE_STREAM, key, value, reply->str);
     freeReplyObject(reply); // Libera la memoria della risposta


     // Piccola dormita tra le operazioni
     client_micro_sleep(500000);
   }
 

    // Chiude la connessione Redis
    redisFree(c2r);
}


