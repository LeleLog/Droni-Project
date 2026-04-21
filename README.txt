```markdown
# 🛰️ Drone Monitoring System

## 📌 Overview

This project is a **centralized drone monitoring system** developed in C++.  
It simulates a set of drones interacting with a control center through a **message-based architecture using Redis**.  
The system stores operational data in a **PostgreSQL database** and provides logging and monitoring capabilities through a server-client structure.

The project was developed in a Linux (Ubuntu) environment.

---

## ⚙️ System Architecture

The system is composed of two main components:

- **Server**
  - Handles drone coordination and system logic
  - Receives and processes messages from clients via Redis
  - Stores logs and operational data in PostgreSQL

- **Client**
  - Simulates drone behavior
  - Sends updates and requests to the server
  - Executes actions based on server instructions

---

## 🧰 Technologies Used

- C++
- Redis (message broker)
- PostgreSQL (database)
- Linux (Ubuntu)
- Make (build system)

---

## 📁 Project Structure

```

Droni/
│
├── client/
│   └── src/        # Client source code
│   └── bin/        # Client executable
│
├── server/
│   └── src/        # Server source code
│   └── bin/        # Server executable
│
└── README.md

````

---

## 🚀 Compilation & Execution

### 1. Build the project

Open two terminals.

#### Client
```bash
cd Droni/client/src
make clean
make
````

#### Server

```bash
cd Droni/server/src
make clean
make
```

---

### 2. Run the system

First start the client:

```bash
cd Droni/client/bin
./main
```

Then start the server:

```bash
cd Droni/server/bin
./main
```

---

## 🗄️ Database Access

To inspect stored data:

```bash
psql postgres
```

Then connect to the database:

```sql
\c logdb_controlcenter
```

Query all drone logs:

```sql
SELECT * FROM DroneLog ORDER BY id;
```

---

## 🧠 Key Features

* Message-based communication between system components
* Simulation of multiple drone entities
* Centralized coordination logic
* Persistent data storage with PostgreSQL
* Modular client-server architecture
* Linux-based build and execution system

---

## 📌 Notes

* The system must be started by launching the client before the server.
* Ensure Redis and PostgreSQL services are running before execution.
* Built and tested on Ubuntu Linux.

---

## 👨‍💻 Author

Computer Science Student – Sapienza University of Rome

```
```
