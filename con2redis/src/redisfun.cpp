#include "con2redis.h"
#include "local.h"

// Funzione per stampare i tipi di risposta di Redis
void print_reply_types()
{

// Stampa i valori numerici associati ai diversi tipi di risposta Redis
  printf("REDIS_REPLY_STRING=%d,\
REDIS_REPLY_STATUS=%d,\
REDIS_REPLY_INTEGER=%d,\
REDIS_REPLY_NIL=%d,\
REDIS_REPLY_ERROR=%d,\
REDIS_REPLY_ARRAY=%d\n",
	 REDIS_REPLY_STRING,
	 REDIS_REPLY_STATUS,
	 REDIS_REPLY_INTEGER,
	 REDIS_REPLY_NIL,
	 REDIS_REPLY_ERROR,
	 REDIS_REPLY_ARRAY
	 );
  
}

// Funzione per verificare il tipo di risposta ricevuta da Redis
void assertReplyType(redisContext *c, redisReply *r, int type) {
    
    if (r == NULL)
        dbg_abort("NULL redisReply (error: %s)", c->errstr);
    if (r->type != type)
      { print_reply_types();
	dbg_abort("Expected reply type %d but got type %d", type, r->type);
      }
}


// Funzione per verificare se la risposta è NULL
void assertReply(redisContext *c, redisReply *r) {
    if (r == NULL)
        dbg_abort("NULL redisReply (error: %s)", c->errstr);
}



// Funzione per stampare i dettagli di una risposta Redis
void dumpReply(redisReply *r, int indent) {

    sds buffer = sdsempty(); // Crea una stringa dinamica vuota per accumulare il messaggio

    switch (r->type) {
        case REDIS_REPLY_STRING:
            buffer = sdscatfmt(buffer, "(string) %s\n", r->str);
            break;
        case REDIS_REPLY_STATUS:
            buffer = sdscatfmt(buffer, "(status) %s\n", r->str);
            break;
        case REDIS_REPLY_INTEGER:
            buffer = sdscatlen(buffer, "(integer) %lld\n", r->integer);
            break;
        case REDIS_REPLY_NIL:
            buffer = sdscatlen(buffer, "(nill)\n", 7);
            break;
        case REDIS_REPLY_ERROR:
            buffer = sdscatfmt(buffer, " (error) %s", r->str);
            break;
        case REDIS_REPLY_ARRAY:
            // Scorre attraverso gli elementi dell'array e stampa ogni risposta con maggiore indentazione
            for (size_t i = 0; i < r->elements; i++) {
                dumpReply(r->element[i], indent + 2);
            }
            break;
        default:
            dbg_abort("Don't know how to deal with reply type %d", r->type);
    }

    // Se il buffer ha contenuto, stampa il contenuto con l'indentazione specificata
    if (sdslen(buffer) > 0) {
        for (int i = 0; i < indent; i++)
            printf(" ");

        printf("%s", buffer);
    }

    sdsfree(buffer); // Libera la memoria usata dal buffer
}


// Funzione per inizializzare uno stream in Redis
void initStreams(redisContext *c, const char *stream) {
    // Crea un gruppo consumer chiamato 'diameter' per lo stream specificato, se non esiste
    redisReply *r = RedisCommand(c, "XGROUP CREATE %s diameter $ MKSTREAM", stream);
    assertReply(c, r); // Verifica che la risposta non sia NULL
    freeReplyObject(r); // Libera la memoria usata dalla risposta
}
