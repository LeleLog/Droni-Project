
-- Crea il database :dbname, eliminandolo se esiste già
drop database if exists :dbname ;
CREATE DATABASE :dbname ;


-- Crea l'utente :username
\c :dbname 

-- Elimina i privilegi e le risorse dell'utente esistente
REASSIGN OWNED BY :username TO postgres ;
REVOKE ALL PRIVILEGES ON ALL TABLES IN SCHEMA public FROM :username ;
REVOKE ALL PRIVILEGES ON ALL SEQUENCES IN SCHEMA public FROM :username ;
REVOKE ALL PRIVILEGES ON ALL FUNCTIONS IN SCHEMA public FROM :username ;
DROP OWNED BY :username;

-- Elimina l'utente se esiste e lo ricrea con una password crittografata
DROP USER IF EXISTS :username ;
CREATE USER :username WITH ENCRYPTED PASSWORD '47002' ;





