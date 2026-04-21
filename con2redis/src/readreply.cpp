#include "con2redis.h"
#include "local.h"


// Funzione che restituisce il numero di stream nella risposta Redis
int ReadNumStreams(redisReply *r)
{
  return (r->elements);  
}  


// Funzione che legge il nome del k-esimo stream e lo copia in streamname
int ReadStreamName(redisReply *r, char *streamname,  long unsigned int k)
{
  strcpy(streamname, r->element[k]->element[0]->str);
  return(0); 
}  

// Funzione che restituisce il numero di messaggi presenti in uno stream specifico
int ReadStreamNumMsg(redisReply *r, long unsigned int streamnum)
{
  
  return(r->element[streamnum]->element[1]->elements);

}  

// Funzione che legge l'ID del messaggio dal k-esimo stream e i-esimo messaggio, copiandolo in msgid
int ReadStreamNumMsgID(redisReply *r, long unsigned int streamnum, int msgnum, char *msgid)
{

  strcpy(msgid, r->element[streamnum]->element[1]->element[msgnum]->element[0]->str);
  return(0);
}


// Funzione che restituisce il numero di valori in un messaggio specifico di uno stream
int ReadStreamMsgNumVal(redisReply *r, long unsigned int streamnum, int msgnum)
{

  return(r->element[streamnum]->element[1]->element[msgnum]->element[1]->elements);

}

// Funzione che legge il valore di un campo specifico in un messaggio di uno stream e lo copia in value
int ReadStreamMsgVal(redisReply *r, long unsigned int streamnum, int msgnum, int entry, char *value)
{

  strcpy(value, r->element[streamnum]->element[1]->element[msgnum]->element[1]->element[entry]->str);
  return(0);
}


