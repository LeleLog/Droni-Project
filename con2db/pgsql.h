
#ifndef pgsql_h
#define pgsql_h

#include <stdio.h> // libreria standard per funzioni di input/output
#include <stdlib.h> // libreria standard per funzioni di allocazione della memoria

//extern "C"
//{
#include <libpq-fe.h> // libreria per la connessione e interazione con un db PostgreSQL
//}


class Con2DB {

private:
 	    
  PGconn *conn; // variabile privata che memorizza il puntatore alla connessione al db

public:

  // Costruttore della classe Con2DB che inizializza una connessione al database utilizzando i parametri forniti
  Con2DB(const char *hostname,
	 const char *port,
	 const char *username,
	 const char *password,
	 const char *dbname);  

  // Funzione per eseguire comandi SQL che non restituiscono dati, come INSERT o UPDATE
  PGresult* ExecSQLcmd(char *sqlcmd);

  // Funzione per eseguire comandi SQL che restituiscono dati, come SELECT
  PGresult* ExecSQLtuples(char *sqlcmd);

  // Funzione per chiudere la connessione al database
  void finish();
  
};


#endif 
