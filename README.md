# Drone Monitoring System (C++ / Redis / PostgreSQL)

This repository contains the implementation of a **centralized drone monitoring system** developed as part of a university project in Computer Science.

The system simulates a set of drones interacting with a **central control unit** through a **message-based architecture using Redis**.  
Operational data is persisted in a **PostgreSQL database** for logging and monitoring purposes.

This project is intended for educational purposes and is not designed for production use.

---

## Project overview

The system is composed of two main components:

- **client/**  
  Simulates drone entities that generate state updates and send them to the control center.

- **server/**  
  Acts as the central control unit, processing incoming messages, coordinating drones, and storing logs into the database.

Communication between components is handled through **Redis messaging**.

---

## System architecture

The system follows a **centralized message-driven architecture**:

- Drones operate as independent simulated entities
- A central server processes all coordination logic
- Redis is used as a message broker between client and server
- PostgreSQL is used for persistent storage of system logs

---

## Project structure

- `client/` contains the drone simulation logic  
  - `src/` source code  
  - `bin/` compiled executable  

- `server/` contains the control center logic  
  - `src/` source code  
  - `bin/` compiled executable  

- `README.md` project documentation  

---

## Technologies used

- C++
- Redis (message broker)
- PostgreSQL (database)
- Linux (Ubuntu environment)
- Make (build system)

---

## How to build

Open two terminals.

### Client
```bash
cd client/src
make clean
make
````

### Server

```bash
cd server/src
make clean
make
```

---

## How to run

Start the client first:

```bash
cd client/bin
./main
```

Then start the server:

```bash
cd server/bin
./main
```

---

## Database access

To inspect stored logs:

```bash
psql postgres
```

Connect to the database:

```sql
\c logdb_controlcenter
```

Query drone logs:

```sql
SELECT * FROM DroneLog ORDER BY id;
```

---

## Key features

* Simulation of multiple drone entities
* Centralized coordination logic
* Message-based communication using Redis
* Persistent logging with PostgreSQL
* Modular client-server architecture
* Linux-based development environment

---

## Notes

* The system must be started by launching the client before the server
* Ensure Redis and PostgreSQL services are running before execution
* The project was developed and tested on Ubuntu Linux

---

## License

This project is for academic purposes. See `LICENSE` for details.

```
