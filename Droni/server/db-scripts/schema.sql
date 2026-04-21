\c :dbname

-- Creazione dei domini per la gestione dei tipi di dati
CREATE DOMAIN Coord AS DOUBLE PRECISION;
CREATE DOMAIN AssignedSector AS INTEGER;
CREATE DOMAIN BatteryLevel AS DOUBLE PRECISION;
CREATE DOMAIN DistanceTraveled AS DOUBLE PRECISION;
CREATE DOMAIN RechargeTimeLeft AS DOUBLE PRECISION;

-- Creazione della tabella DroneLog se non esiste
CREATE TABLE IF NOT EXISTS DroneLog (
     
     -- Colonne dati di ogni drone
     id INT PRIMARY KEY,  -- Identificatore unico per ogni drone
     pos_x Coord NOT NULL, -- Posizione X del drone
     pos_y Coord NOT NULL, -- Posizione Y del drone
     sector AssignedSector NOT NULL,  -- Settore assegnato al drone
     distance DistanceTraveled NOT NULL,  -- Distanza totale percorsa dal drone
     battery BatteryLevel NOT NULL,  -- Livello attuale della batteria del drone 
     rechargetime RechargeTimeLeft NOT NULL,  -- Tempo rimanente per completare la ricarica del drone
     pid INT NOT NULL, -- ID del processo
     time TIMESTAMP DEFAULT CURRENT_TIMESTAMP,  -- Momento in cui il log è stato registrato
     
     -- Colonne per i risultati dei monitor (proprietà funzionali e non)
     position_check INT DEFAULT 0,  -- 0 = OK, 1 = ERROR, PROPRIETÀ FUNZIONALE
     sector_assignment_check INT DEFAULT 0, -- 0 = OK, 1 = ERROR, PROPRIETÀ FUNZIONALE
     battery_consumption_check INT DEFAULT 0, -- 0 = OK, 1 = ERROR, PROPRIETÀ FUNZIONALE
     recharge_time_check INT DEFAULT 0, -- 0 = OK, 1 = ERROR, PROPRIETÀ NON FUNZIONALE 
     log_integrity_check INT DEFAULT 0 -- 0 = OK, 1 = ERROR, PROPRIETÀ NON FUNZIONALE


);




