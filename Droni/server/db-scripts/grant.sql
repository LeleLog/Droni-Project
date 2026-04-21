\c :dbname 

-- Concede tutti i privilegi sull'intero database all'utente
GRANT ALL PRIVILEGES ON DATABASE :dbname to :username ;

-- Concede tutti i privilegi sullo schema pubblico all'utente
GRANT ALL ON SCHEMA public TO :username ;

-- Concede all'utente il permesso di utilizzare e aggiornare tutte le sequenze nello schema
GRANT USAGE, SELECT, UPDATE ON ALL SEQUENCES IN SCHEMA public TO :username;

-- Cambia il proprietario della tabella nell'utente
ALTER TABLE DroneLog OWNER TO :username ;



