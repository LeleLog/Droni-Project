
#include "pgsql.h"

#define DEBUG 1000


Con2DB::Con2DB(const char *hostname,
	       const char *port,
	 const char *username,
	 const char *password,
	 const char *dbname) 
{

  char buf[1000]; // Array di caratteri per contenere la stringa di connessione
  
  // Costruisce la stringa di connessione con i parametri forniti
  sprintf(buf, "host=%s port=%s user=%s password=%s dbname=%s",
	  hostname,
	  port,
	  username,
	  password,
	  dbname
	  );
  
  // Connette l'utente al database usando la stringa costruita
      conn = PQconnectdb(buf);


   // Controlla se la connessione al database è avvenuta con successo
    if (PQstatus(conn) != CONNECTION_OK) {
        
        fprintf(stderr, "Con2DB(%s): Connection to database failed: %s\n",
		dbname,
		PQerrorMessage(conn)
		);
        finish();
    }

#if 0
    fprintf(stderr, "Con2DB(%s): PQconnectPoll: %d\n", dbname, PQconnectPoll(conn));        
    fprintf(stderr, "Con2DB(%s): PQstatus: %d\n", dbname, PQstatus(conn));       
    fprintf(stderr, "Con2DB(%s): Connection to DB successfully established \n", dbname);        
#endif

   
}  /* Con2DB() */


// Funzione per terminare la connessione al database e uscire dal programma
void Con2DB::finish() {
    
    PQfinish(conn); // Chiude la connessione al database
    conn = NULL;  // Imposta il puntatore a NULL
    exit(1);  // Termina il programma con codice di uscita 1
}



// Funzione per eseguire comandi SQL che non restituiscono dati, come INSERT o UPDATE
PGresult* Con2DB::ExecSQLcmd(char *sqlcmd)
{
  // Verifica se la connessione al db è attiva
  if (conn == NULL)
    // errore
    {
     fprintf(stderr, "ExecSQLcmd(): no connection to DB: PQexec on: %s\n", sqlcmd);
          exit (1);
    }
  
#if (DEBUG > 1000000)
  fprintf(stderr, "ExecSQLcmd(): PQexec on: %s\n", sqlcmd);        
  fprintf(stderr, "ExecSQLcmd(): PQconnectPoll: %d\n", PQconnectPoll(conn));        
  fprintf(stderr, "ExecSQLcmd(): PQstatus: %d\n", PQstatus(conn));        
#endif

  // Esegue il comando SQL usando PQexec e memorizza il risultato in res
  PGresult *res = PQexec(conn, sqlcmd);    

#if (DEBUG > 1000000)
    fprintf(stderr, "ExecSQLcmd(): PQexec done\n");        
#endif
    
    // Verifica se il comando SQL è stato eseguito correttamente
    if (PQresultStatus(res) != PGRES_COMMAND_OK) {

      fprintf(stderr, "ExecSQLcmd(): SQL command failed: %s\n", sqlcmd);
      fprintf(stderr, "ExecSQLcmd(): %s\n",
	      PQresStatus(PQresultStatus(res)));
      fprintf(stderr, "ExecSQLcmd(): PQresultErrorMessage: %s\n",
	      PQresultErrorMessage(res) ) ;
           
        PQclear(res); // Libera la memoria del risultato
        finish();
    }    

#if 0
    fprintf(stderr, "ExecSQLcmd() SQL command OK: %s\n", sqlcmd);        
#endif

    return (res);
}


// Funzione per eseguire comandi SQL che restituiscono dati, come SELECT
PGresult* Con2DB::ExecSQLtuples(char *sqlcmd)
{
  // Verifica se la connessione al db è attiva
  if (conn == NULL)
    // errore
    {
     fprintf(stderr, "ExecSQLtuples(): no connection to DB: sqlcmd = %s\n", sqlcmd);
     exit (1);
    }

  
  // Esegue il comando SQL usando PQexec e memorizza il risultato in res
  PGresult *res = PQexec(conn, sqlcmd);   
  
  // Verifica se il comando SQL è stato eseguito correttamente e restituisce tuple di dati
    if (PQresultStatus(res) != PGRES_TUPLES_OK) {

      fprintf(stderr, "ExecSQLtuples(): SQL command failed: %s\n", sqlcmd);
      fprintf(stderr, "ExecSQLtuples(): %s\n",
	      PQresStatus(PQresultStatus(res)));

        PQclear(res); // Libera la memoria del risultato 
        finish();
    }    

#if 0
    fprintf(stderr, "ExecSQLtuples() OK: %s\n", sqlcmd);        
#endif

    return (res);
}
