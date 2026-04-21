#ifndef con2redis_h
#define con2redis_h

#include <stdio.h>
#include <stdlib.h>

// include hiredis
extern "C" {
#include <hiredis/hiredis.h>
}



#define dbg_log(fmt, ... )			\
    do { \
        fprintf(stderr, "%s:%d : " fmt "\n", __FILE__, __LINE__,__VA_ARGS__); \
    } while (0);


#define dbg_abort(fmt, ...) \
    do { \
        dbg_log(fmt, __VA_ARGS__); exit(-1); \
    } while (0)


#define RedisCommand(fmt, ...)			\
  (redisReply*) redisCommand(fmt, __VA_ARGS__)

// Funzione per verificare il tipo di risposta ricevuta da Redis
void assertReplyType(redisContext *c, redisReply *r, int type);

// Funzione per verificare se la risposta è NULL
void assertReply(redisContext *c, redisReply *r);

// Funzione per stampare i dettagli di una risposta Redis
void dumpReply(redisReply *r, int indent);

// Funzione per inizializzare uno stream in Redis
void initStreams(redisContext *c, const char *stream);

//
void ReadStreamReply(redisReply *r, int indent, int readstatus);

// Funzione che legge il nome del k-esimo stream e lo copia in streamname
int ReadStreamName(redisReply *r, char *streamname, long unsigned int k);

// Funzione che restituisce il numero di stream nella risposta Redis
int ReadNumStreams(redisReply *r);

// Funzione che restituisce il numero di messaggi presenti in uno stream specifico
int ReadStreamNumMsg(redisReply *r, long unsigned int streamnum);

// Funzione che legge l'ID del messaggio dal k-esimo stream e i-esimo messaggio, copiandolo in msgid
int ReadStreamNumMsgID(redisReply *r, long unsigned int streamnum, int msgnum, char *msgid);

// Funzione che restituisce il numero di valori in un messaggio specifico di uno stream
int ReadStreamMsgNumVal(redisReply *r, long unsigned int streamnum, int msgnum);

// Funzione che legge il valore di un campo specifico in un messaggio di uno stream e lo copia in value
int ReadStreamMsgVal(redisReply *r, long unsigned int streamnum, int msgnum, int entry, char *fval);

#endif
