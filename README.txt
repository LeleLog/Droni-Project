Passaggi per compilazione ed esecuzione del sistema:

1) Aprire in due finestre differenti il progetto
2) Aprire la cartella Droni in entrambe le finestre
3) In una finestra aprire la cartella server e nell'altra la cartella client
4) Dalla cartella client, aprire la cartella src contente i file sorgenti. Da questo percorso qui, aprire il terminale ed effettuare prima l'istruzione make clean e poi l'istruzione make
5) Dalla cartella server, aprire la cartella src contente i file sorgenti. Da questo percorso qui, aprire il terminale ed effettuare prima l'istruzione make clean e poi l'istruzione make
6) Dopo aver eseguito la compilazione per entrambi i processi, dirigersi in una finestra, nella cartella bin per il client e nell'altra finestra, nella cartella bin per il server. Qui sono presenti gli eseguibili dei due processi
7) Aprire il terminale prima nella cartella bin del client ed eseguire il file tramite l'istruzione ./main
8) Aprire il terminale nella cartella bin del server ed eseguire il file tramite l'istruzione ./main

Nota: per terminare l'esecuzione di uno dei due processi premere la combinazione di tasti ctrl+c

Passaggi per visualizzare i dati all'interno del database:

1) Dirigersi nella cartella server e poi aprire la cartella src
2) Da questo percorso, aprire il terminale e inserire l'istruzione psql postgres
3) Dopodiché eseguire la seguente istruzione: \c logdb_controlcenter (nome del database)
4) Effettuare la seguente query per visualizzare tutti i dati dei droni e i controlli effettuati dai monitor: 
   select * from DroneLog order by id;