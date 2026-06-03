# PostgreSQL Migration + Bug Fixes — Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Replace text-file persistence with PostgreSQL, fix all known bugs, and push to GitHub `lougetrackerv2`.

**Architecture:** Keep existing in-memory data structures. On startup load from PostgreSQL instead of text files. Every mutation (register, reserve, pay, cancel) writes to PostgreSQL immediately via libpqxx. HTTP handlers and domain classes stay structurally unchanged.

**Tech Stack:** C++17, libpqxx 7.x, OpenSSL SHA-256, Docker, docker-compose, Render PostgreSQL

---

## File Map

| Action | Path | Responsibility |
|--------|------|----------------|
| Create | `backend/include/Database.h` | pqxx singleton wrapper |
| Create | `backend/src/Database.cpp` | reads `DATABASE_URL`, opens connection |
| Create | `sql/schema.sql` | DDL + seed data (replaces both .txt files) |
| Create | `docker-compose.yml` | local dev: postgres + server containers |
| Create | `backend/.env.example` | documents required env vars |
| Modify | `backend/src/AccountManager.cpp` | SHA-256 hash, loadFromDB, per-op DB writes |
| Modify | `backend/include/AccountManager.h` | add loadFromDB declaration |
| Modify | `backend/src/System.cpp` | loadFromDB |
| Modify | `backend/include/System.h` | add loadFromDB declaration |
| Modify | `backend/src/web_server_main.cpp` | DB writes for reservations, session fix, buffer fix, startup |
| Modify | `backend/Dockerfile` | add libpqxx-dev libssl-dev, fix compile flags |
| Create | `.gitignore` | exclude build artifacts, .env |

---

## Task 1: Git repository initialisation

**Files:** `.gitignore` at `LOUAGE_TRACKER/`

- [ ] **Step 1: Initialise git**

```bash
cd "M:/LENOVO/Desktop/Projects/louage finder/LOUAGE_TRACKER"
git init
```

Expected: `Initialized empty Git repository in .../LOUAGE_TRACKER/.git/`

- [ ] **Step 2: Create .gitignore**

Create `M:/LENOVO/Desktop/Projects/louage finder/LOUAGE_TRACKER/.gitignore`:

```
# Build artefacts
backend/*.exe
backend/server_local.exe
backend/build_errors.txt
backend/build/

# Env
.env
backend/.env

# Editor
.vscode/
*.layout

# Object files
*.o
```

- [ ] **Step 3: Initial commit**

```bash
git add .
git commit -m "chore: initial commit — existing C++ louage tracker"
```

Expected: commit hash printed, no errors.

---

## Task 2: docker-compose for local development

**Files:** Create `docker-compose.yml` at `LOUAGE_TRACKER/`

- [ ] **Step 1: Create docker-compose.yml**

```yaml
version: '3.8'
services:
  db:
    image: postgres:15-alpine
    environment:
      POSTGRES_DB: louage_tracker
      POSTGRES_USER: louage_user
      POSTGRES_PASSWORD: louage_pass
    ports:
      - "5432:5432"
    volumes:
      - ./sql/schema.sql:/docker-entrypoint-initdb.d/schema.sql
      - pgdata:/var/lib/postgresql/data

  server:
    build: ./backend
    environment:
      DATABASE_URL: postgresql://louage_user:louage_pass@db:5432/louage_tracker
    ports:
      - "8080:8080"
    depends_on:
      - db
    restart: on-failure

volumes:
  pgdata:
```

- [ ] **Step 2: Create backend/.env.example**

```
DATABASE_URL=postgresql://louage_user:louage_pass@localhost:5432/louage_tracker
```

- [ ] **Step 3: Commit**

```bash
git add docker-compose.yml backend/.env.example
git commit -m "chore: add docker-compose for local dev"
```

---

## Task 3: Database singleton

**Files:**
- Create: `backend/include/Database.h`
- Create: `backend/src/Database.cpp`

- [ ] **Step 1: Create Database.h**

```cpp
#ifndef DATABASE_H
#define DATABASE_H

#include <pqxx/pqxx>

class Database {
public:
    static pqxx::connection& get();
private:
    static pqxx::connection* conn;
    Database() = delete;
};

#endif
```

- [ ] **Step 2: Create Database.cpp**

```cpp
#include "Database.h"
#include <cstdlib>
#include <stdexcept>
#include <iostream>

pqxx::connection* Database::conn = nullptr;

pqxx::connection& Database::get() {
    if (!conn) {
        const char* url = std::getenv("DATABASE_URL");
        if (!url) throw std::runtime_error("DATABASE_URL environment variable not set");
        conn = new pqxx::connection(url);
        std::cout << "✓ Connexion PostgreSQL établie" << std::endl;
    }
    return *conn;
}
```

- [ ] **Step 3: Commit**

```bash
git add backend/include/Database.h backend/src/Database.cpp
git commit -m "feat: add Database singleton wrapping libpqxx connection"
```

---

## Task 4: SQL schema and seed data

**Files:** Create `sql/schema.sql`

- [ ] **Step 1: Compute SHA-256 hash of seed password**

Run in a shell (Linux/WSL/Git Bash):
```bash
echo -n "stored" | openssl dgst -sha256 | awk '{print $2}'
```

Copy the output hash — it will be used as `<HASH>` in the next step.

- [ ] **Step 2: Create sql/schema.sql**

Replace `<HASH>` with the value from Step 1.

```sql
-- Louage Tracker — PostgreSQL schema
-- Run once: psql $DATABASE_URL -f sql/schema.sql

CREATE EXTENSION IF NOT EXISTS pgcrypto;

-- ── Tables ──────────────────────────────────────────────────────────────────

CREATE TABLE IF NOT EXISTS users (
    id               SERIAL PRIMARY KEY,
    username         VARCHAR(50)  UNIQUE NOT NULL,
    password_hash    VARCHAR(255) NOT NULL,
    email            VARCHAR(100) UNIQUE NOT NULL,
    phone            VARCHAR(20),
    role             VARCHAR(20)  NOT NULL,
    full_name        VARCHAR(100),
    license_number   VARCHAR(50),
    experience_years INT,
    station_name     VARCHAR(100),
    permission_level INT DEFAULT 3,
    created_at       TIMESTAMPTZ DEFAULT NOW()
);

CREATE TABLE IF NOT EXISTS stations (
    id   SERIAL PRIMARY KEY,
    name VARCHAR(100) UNIQUE NOT NULL
);

CREATE TABLE IF NOT EXISTS destinations (
    id         SERIAL PRIMARY KEY,
    station_id INT REFERENCES stations(id) ON DELETE CASCADE,
    name       VARCHAR(100) NOT NULL,
    UNIQUE (station_id, name)
);

CREATE TABLE IF NOT EXISTS louages (
    id             SERIAL PRIMARY KEY,
    destination_id INT REFERENCES destinations(id) ON DELETE CASCADE,
    serie_vehicule INT NOT NULL,
    numero_louage  INT NOT NULL,
    id_prop        INT NOT NULL,
    occupied_seats INT DEFAULT 0
);

CREATE TABLE IF NOT EXISTS reservations (
    id                 SERIAL PRIMARY KEY,
    reservation_number INT UNIQUE NOT NULL,
    user_id            INT NOT NULL DEFAULT 0,
    passenger_name     VARCHAR(100) NOT NULL,
    destination_id     INT REFERENCES destinations(id) ON DELETE CASCADE,
    is_paid            BOOLEAN DEFAULT FALSE,
    times_called       INT DEFAULT 0,
    created_at         TIMESTAMPTZ DEFAULT NOW()
);

-- ── Seed: Users ──────────────────────────────────────────────────────────────
-- Password for all seed accounts: "stored"  (SHA-256 hash below)

INSERT INTO users (id, username, password_hash, email, phone, role) VALUES
(1000, 'ahmed',           '<HASH>', 'ahmed@example.com',      '98765432', 'PASSENGER'),
(1001, 'fatima',          '<HASH>', 'fatima@example.com',     '97654321', 'PASSENGER'),
(1002, 'karim',           '<HASH>', 'karim@example.com',      '96543210', 'PASSENGER'),
(1003, 'ali_driver',      '<HASH>', 'ali@example.com',        '95432109', 'DRIVER'),
(1004, 'sami_driver',     '<HASH>', 'sami@example.com',       '94321098', 'DRIVER'),
(1005, 'cashier_tunis',   '<HASH>', 'cashier1@example.com',   '93210987', 'CASHIER'),
(1006, 'cashier_sousse',  '<HASH>', 'cashier2@example.com',   '92109876', 'CASHIER'),
(1007, 'manager',         '<HASH>', 'manager@example.com',    '91098765', 'MANAGER'),
(1008, 'admin',           '<HASH>', 'admin@example.com',      '90987654', 'ADMIN')
ON CONFLICT DO NOTHING;

-- driver extra fields
UPDATE users SET license_number='DRV001', experience_years=10 WHERE username='ali_driver';
UPDATE users SET license_number='DRV002', experience_years=5  WHERE username='sami_driver';
-- cashier extra fields
UPDATE users SET station_name='Tunis'  WHERE username='cashier_tunis';
UPDATE users SET station_name='Sousse' WHERE username='cashier_sousse';
-- admin extra fields
UPDATE users SET permission_level=3 WHERE username='admin';

SELECT setval('users_id_seq', (SELECT MAX(id) FROM users));

-- ── Seed: Stations ───────────────────────────────────────────────────────────

INSERT INTO stations (id, name) VALUES
(1,'Tunis'),(2,'Sousse'),(3,'Sfax'),(4,'Gabès'),
(5,'Medenine'),(8,'Kairouan'),(9,'Bizerte'),(10,'Nabeul'),
(11,'Monastir'),(12,'Mahdia')
ON CONFLICT DO NOTHING;

SELECT setval('stations_id_seq', (SELECT MAX(id) FROM stations));

-- ── Seed: Destinations ───────────────────────────────────────────────────────

INSERT INTO destinations (station_id, name) VALUES
-- Tunis
(1,'Sousse'),(1,'Sfax'),(1,'Gabès'),(1,'Bizerte'),
(1,'Nabeul'),(1,'Kairouan'),(1,'Monastir'),(1,'Mahdia'),
-- Sousse
(2,'Tunis'),(2,'Sfax'),(2,'Monastir'),(2,'Mahdia'),(2,'Kairouan'),(2,'Nabeul'),
-- Sfax
(3,'Tunis'),(3,'Sousse'),(3,'Gabès'),(3,'Kairouan'),(3,'Mahdia'),
-- Gabès
(4,'Tunis'),(4,'Sfax'),(4,'Medenine'),
-- Medenine
(5,'Tunis'),(5,'Gabès'),(5,'Sfax'),
-- Kairouan
(8,'Tunis'),(8,'Sousse'),(8,'Sfax'),
-- Bizerte
(9,'Tunis'),(9,'Béja'),
-- Nabeul
(10,'Tunis'),(10,'Sousse'),
-- Monastir
(11,'Tunis'),(11,'Sousse'),(11,'Mahdia'),
-- Mahdia
(12,'Tunis'),(12,'Sousse'),(12,'Sfax'),(12,'Monastir')
ON CONFLICT DO NOTHING;

-- ── Seed: Louages ────────────────────────────────────────────────────────────

INSERT INTO louages (destination_id, serie_vehicule, numero_louage, id_prop)
SELECT d.id, 1, 2050, 1001 FROM destinations d JOIN stations s ON s.id=d.station_id WHERE s.name='Tunis' AND d.name='Sousse';
INSERT INTO louages (destination_id, serie_vehicule, numero_louage, id_prop)
SELECT d.id, 1, 2051, 1002 FROM destinations d JOIN stations s ON s.id=d.station_id WHERE s.name='Tunis' AND d.name='Sousse';
INSERT INTO louages (destination_id, serie_vehicule, numero_louage, id_prop)
SELECT d.id, 2, 3050, 2001 FROM destinations d JOIN stations s ON s.id=d.station_id WHERE s.name='Tunis' AND d.name='Sfax';
INSERT INTO louages (destination_id, serie_vehicule, numero_louage, id_prop)
SELECT d.id, 3, 4050, 3001 FROM destinations d JOIN stations s ON s.id=d.station_id WHERE s.name='Tunis' AND d.name='Gabès';
INSERT INTO louages (destination_id, serie_vehicule, numero_louage, id_prop)
SELECT d.id, 4, 1050, 4001 FROM destinations d JOIN stations s ON s.id=d.station_id WHERE s.name='Tunis' AND d.name='Bizerte';
INSERT INTO louages (destination_id, serie_vehicule, numero_louage, id_prop)
SELECT d.id, 5, 1150, 5001 FROM destinations d JOIN stations s ON s.id=d.station_id WHERE s.name='Tunis' AND d.name='Nabeul';
INSERT INTO louages (destination_id, serie_vehicule, numero_louage, id_prop)
SELECT d.id, 6, 2250, 11001 FROM destinations d JOIN stations s ON s.id=d.station_id WHERE s.name='Tunis' AND d.name='Kairouan';
INSERT INTO louages (destination_id, serie_vehicule, numero_louage, id_prop)
SELECT d.id, 7, 1250, 8001 FROM destinations d JOIN stations s ON s.id=d.station_id WHERE s.name='Tunis' AND d.name='Monastir';
INSERT INTO louages (destination_id, serie_vehicule, numero_louage, id_prop)
SELECT d.id, 8, 1270, 8101 FROM destinations d JOIN stations s ON s.id=d.station_id WHERE s.name='Tunis' AND d.name='Mahdia';
INSERT INTO louages (destination_id, serie_vehicule, numero_louage, id_prop)
SELECT d.id, 9, 2150, 6001 FROM destinations d JOIN stations s ON s.id=d.station_id WHERE s.name='Sousse' AND d.name='Tunis';
INSERT INTO louages (destination_id, serie_vehicule, numero_louage, id_prop)
SELECT d.id, 10, 3150, 7001 FROM destinations d JOIN stations s ON s.id=d.station_id WHERE s.name='Sousse' AND d.name='Sfax';
INSERT INTO louages (destination_id, serie_vehicule, numero_louage, id_prop)
SELECT d.id, 15, 3250, 9001 FROM destinations d JOIN stations s ON s.id=d.station_id WHERE s.name='Sfax' AND d.name='Tunis';
INSERT INTO louages (destination_id, serie_vehicule, numero_louage, id_prop)
SELECT d.id, 17, 4250, 9501 FROM destinations d JOIN stations s ON s.id=d.station_id WHERE s.name='Sfax' AND d.name='Gabès';
INSERT INTO louages (destination_id, serie_vehicule, numero_louage, id_prop)
SELECT d.id, 20, 4350, 10001 FROM destinations d JOIN stations s ON s.id=d.station_id WHERE s.name='Gabès' AND d.name='Tunis';
INSERT INTO louages (destination_id, serie_vehicule, numero_louage, id_prop)
SELECT d.id, 22, 5050, 13001 FROM destinations d JOIN stations s ON s.id=d.station_id WHERE s.name='Gabès' AND d.name='Medenine';
INSERT INTO louages (destination_id, serie_vehicule, numero_louage, id_prop)
SELECT d.id, 35, 1050, 4003 FROM destinations d JOIN stations s ON s.id=d.station_id WHERE s.name='Bizerte' AND d.name='Tunis';
INSERT INTO louages (destination_id, serie_vehicule, numero_louage, id_prop)
SELECT d.id, 23, 1350, 12001 FROM destinations d JOIN stations s ON s.id=d.station_id WHERE s.name='Nabeul' AND d.name='Tunis';
INSERT INTO louages (destination_id, serie_vehicule, numero_louage, id_prop)
SELECT d.id, 25, 2250, 11001 FROM destinations d JOIN stations s ON s.id=d.station_id WHERE s.name='Kairouan' AND d.name='Tunis';
INSERT INTO louages (destination_id, serie_vehicule, numero_louage, id_prop)
SELECT d.id, 28, 1250, 8003 FROM destinations d JOIN stations s ON s.id=d.station_id WHERE s.name='Monastir' AND d.name='Tunis';
INSERT INTO louages (destination_id, serie_vehicule, numero_louage, id_prop)
SELECT d.id, 37, 5150, 13101 FROM destinations d JOIN stations s ON s.id=d.station_id WHERE s.name='Medenine' AND d.name='Tunis';

-- No reservation seed data — reservations are created at runtime

-- ── Reset sequences ──────────────────────────────────────────────────────────
SELECT setval('reservations_reservation_number_seq', 2000) WHERE EXISTS (SELECT 1 FROM reservations);
```

- [ ] **Step 3: Commit**

```bash
git add sql/schema.sql
git commit -m "feat: add PostgreSQL schema and seed data"
```

---

## Task 5: Update Dockerfile

**Files:** Modify `backend/Dockerfile`

- [ ] **Step 1: Replace Dockerfile content**

```dockerfile
FROM gcc:12-bookworm

WORKDIR /app

RUN apt-get update && apt-get install -y \
    libpqxx-dev \
    libssl-dev \
    && rm -rf /var/lib/apt/lists/*

COPY . .

RUN g++ -o server \
    src/web_server_main.cpp \
    src/AccountManager.cpp \
    src/User.cpp \
    src/Admin.cpp \
    src/Cashier.cpp \
    src/Driver.cpp \
    src/Manager.cpp \
    src/Passenger.cpp \
    src/Destination.cpp \
    src/Louage.cpp \
    src/reservation.cpp \
    src/ReservationSystem.cpp \
    src/ReservationQueue.cpp \
    src/Station.cpp \
    src/System.cpp \
    src/html_templates.cpp \
    src/Database.cpp \
    -Iinclude -Isrc \
    -std=c++17 \
    -lpqxx -lpq -lssl -lcrypto \
    -pthread

EXPOSE 8080

CMD ["./server"]
```

- [ ] **Step 2: Commit**

```bash
git add backend/Dockerfile
git commit -m "chore: update Dockerfile — add libpqxx-dev libssl-dev"
```

---

## Task 6: Fix password hashing (SHA-256)

**Files:** Modify `backend/src/AccountManager.cpp`, `backend/include/AccountManager.h`

The existing XOR `hashPassword` is replaced with OpenSSL SHA-256.

- [ ] **Step 1: Add OpenSSL include to AccountManager.cpp**

At the top of `backend/src/AccountManager.cpp`, add after existing includes:

```cpp
#include <openssl/sha.h>
#include <sstream>
#include <iomanip>
```

- [ ] **Step 2: Replace hashPassword in AccountManager.cpp**

Find the existing `hashPassword` function (the XOR one) and replace it entirely:

```cpp
string AccountManager::hashPassword(const string& password) const {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256(reinterpret_cast<const unsigned char*>(password.c_str()),
           password.size(), hash);
    stringstream ss;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++)
        ss << hex << setw(2) << setfill('0') << static_cast<int>(hash[i]);
    return ss.str();
}
```

Also replace the same XOR `hashPassword` in `backend/src/User.cpp` (the local `static` one) with the same body — or delete it and call `AccountManager::hashPassword` through the login flow instead. The cleanest fix: delete the static function in `User.cpp` and update `User::login` to compare using the same SHA-256 helper:

In `backend/src/User.cpp`, replace the static `hashPassword` function and `User::login`:

```cpp
#include <openssl/sha.h>
#include <sstream>
#include <iomanip>

static string sha256Hex(const string& input) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256(reinterpret_cast<const unsigned char*>(input.c_str()),
           input.size(), hash);
    stringstream ss;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++)
        ss << hex << setw(2) << setfill('0') << static_cast<int>(hash[i]);
    return ss.str();
}

bool User::login(string uname, string pwd) {
    if (username == uname && password == sha256Hex(pwd)) {
        cout << "✓ Connexion réussie: " << username << " (" << role << ")" << endl;
        return true;
    }
    return false;
}
```

- [ ] **Step 3: Commit**

```bash
git add backend/src/AccountManager.cpp backend/src/User.cpp
git commit -m "fix: replace XOR password hash with OpenSSL SHA-256"
```

---

## Task 7: AccountManager — loadFromDB

**Files:** Modify `backend/include/AccountManager.h`, `backend/src/AccountManager.cpp`

- [ ] **Step 1: Add loadFromDB declaration to AccountManager.h**

In the `// File Operations` section of `AccountManager.h`, add:

```cpp
bool loadFromDB();
```

- [ ] **Step 2: Add includes to AccountManager.cpp**

At the top of `AccountManager.cpp`, add:

```cpp
#include "Database.h"
#include <pqxx/pqxx>
```

- [ ] **Step 3: Add loadFromDB implementation to AccountManager.cpp**

```cpp
bool AccountManager::loadFromDB() {
    try {
        pqxx::work txn(Database::get());
        pqxx::result rows = txn.exec(
            "SELECT id, username, password_hash, email, phone, role, "
            "full_name, license_number, experience_years, station_name, permission_level "
            "FROM users ORDER BY id");

        for (const auto& row : rows) {
            int    id       = row["id"].as<int>();
            string username = row["username"].c_str();
            string pwd      = row["password_hash"].c_str();
            string email    = row["email"].c_str();
            string phone    = row["phone"].is_null() ? "" : row["phone"].c_str();
            string role     = row["role"].c_str();

            if (id >= nextUserID) nextUserID = id + 1;

            if (role == "PASSENGER") {
                string fn = row["full_name"].is_null() ? "" : row["full_name"].c_str();
                passengers.push_back(new Passenger(id, username, pwd, email, phone, fn));
            } else if (role == "DRIVER") {
                string lic = row["license_number"].is_null() ? "" : row["license_number"].c_str();
                int    exp = row["experience_years"].is_null() ? 0 : row["experience_years"].as<int>();
                drivers.push_back(new Driver(id, username, pwd, email, phone, lic, exp));
            } else if (role == "CASHIER") {
                string st = row["station_name"].is_null() ? "" : row["station_name"].c_str();
                cashiers.push_back(new Cashier(id, username, pwd, email, phone, st));
            } else if (role == "MANAGER") {
                managers.push_back(new Manager(id, username, pwd, email, phone));
            } else if (role == "ADMIN") {
                int perm = row["permission_level"].is_null() ? 3 : row["permission_level"].as<int>();
                admins.push_back(new Admin(id, username, pwd, email, phone, perm));
            }
        }
        txn.commit();
        cout << "✓ " << getTotalUsers() << " compte(s) chargé(s) depuis PostgreSQL" << endl;
        return true;
    } catch (const exception& e) {
        cerr << "✗ Erreur loadFromDB (accounts): " << e.what() << endl;
        return false;
    }
}
```

- [ ] **Step 4: Commit**

```bash
git add backend/include/AccountManager.h backend/src/AccountManager.cpp
git commit -m "feat: AccountManager::loadFromDB — load users from PostgreSQL"
```

---

## Task 8: AccountManager — DB writes on mutations

**Files:** Modify `backend/src/AccountManager.cpp`

Replace the body of each `registerX` method so it inserts into PostgreSQL first, then updates the in-memory vector. Also update `deleteAccount` and `changeUserPassword`.

- [ ] **Step 1: Replace registerPassenger**

```cpp
bool AccountManager::registerPassenger(string username, string password,
                                        string email, string phone, string fullName) {
    if (usernameExists(username)) { cout << "✗ Username déjà utilisé" << endl; return false; }
    if (emailExists(email))       { cout << "✗ Email déjà utilisé"    << endl; return false; }

    string pwd = hashPassword(password);
    try {
        pqxx::work txn(Database::get());
        pqxx::result r = txn.exec_params(
            "INSERT INTO users (username,password_hash,email,phone,role,full_name) "
            "VALUES ($1,$2,$3,$4,'PASSENGER',$5) RETURNING id",
            username, pwd, email, phone, fullName);
        int newId = r[0]["id"].as<int>();
        txn.commit();
        passengers.push_back(new Passenger(newId, username, pwd, email, phone, fullName));
        if (newId >= nextUserID) nextUserID = newId + 1;
        cout << "✓ Compte passager créé (ID " << newId << ")" << endl;
        return true;
    } catch (const exception& e) {
        cerr << "✗ DB registerPassenger: " << e.what() << endl;
        return false;
    }
}
```

- [ ] **Step 2: Replace registerDriver**

```cpp
bool AccountManager::registerDriver(string username, string password, string email,
                                     string phone, string license, int experience) {
    if (usernameExists(username) || emailExists(email)) return false;
    string pwd = hashPassword(password);
    try {
        pqxx::work txn(Database::get());
        pqxx::result r = txn.exec_params(
            "INSERT INTO users (username,password_hash,email,phone,role,license_number,experience_years) "
            "VALUES ($1,$2,$3,$4,'DRIVER',$5,$6) RETURNING id",
            username, pwd, email, phone, license, experience);
        int newId = r[0]["id"].as<int>();
        txn.commit();
        drivers.push_back(new Driver(newId, username, pwd, email, phone, license, experience));
        if (newId >= nextUserID) nextUserID = newId + 1;
        return true;
    } catch (const exception& e) {
        cerr << "✗ DB registerDriver: " << e.what() << endl;
        return false;
    }
}
```

- [ ] **Step 3: Replace registerCashier**

```cpp
bool AccountManager::registerCashier(string username, string password, string email,
                                      string phone, string station) {
    if (usernameExists(username) || emailExists(email)) return false;
    string pwd = hashPassword(password);
    try {
        pqxx::work txn(Database::get());
        pqxx::result r = txn.exec_params(
            "INSERT INTO users (username,password_hash,email,phone,role,station_name) "
            "VALUES ($1,$2,$3,$4,'CASHIER',$5) RETURNING id",
            username, pwd, email, phone, station);
        int newId = r[0]["id"].as<int>();
        txn.commit();
        cashiers.push_back(new Cashier(newId, username, pwd, email, phone, station));
        if (newId >= nextUserID) nextUserID = newId + 1;
        return true;
    } catch (const exception& e) {
        cerr << "✗ DB registerCashier: " << e.what() << endl;
        return false;
    }
}
```

- [ ] **Step 4: Replace registerManager**

```cpp
bool AccountManager::registerManager(string username, string password,
                                      string email, string phone) {
    if (usernameExists(username) || emailExists(email)) return false;
    string pwd = hashPassword(password);
    try {
        pqxx::work txn(Database::get());
        pqxx::result r = txn.exec_params(
            "INSERT INTO users (username,password_hash,email,phone,role) "
            "VALUES ($1,$2,$3,$4,'MANAGER') RETURNING id",
            username, pwd, email, phone);
        int newId = r[0]["id"].as<int>();
        txn.commit();
        managers.push_back(new Manager(newId, username, pwd, email, phone));
        if (newId >= nextUserID) nextUserID = newId + 1;
        return true;
    } catch (const exception& e) {
        cerr << "✗ DB registerManager: " << e.what() << endl;
        return false;
    }
}
```

- [ ] **Step 5: Replace registerAdmin**

```cpp
bool AccountManager::registerAdmin(string username, string password,
                                    string email, string phone, int permLevel) {
    if (usernameExists(username) || emailExists(email)) return false;
    string pwd = hashPassword(password);
    try {
        pqxx::work txn(Database::get());
        pqxx::result r = txn.exec_params(
            "INSERT INTO users (username,password_hash,email,phone,role,permission_level) "
            "VALUES ($1,$2,$3,$4,'ADMIN',$5) RETURNING id",
            username, pwd, email, phone, permLevel);
        int newId = r[0]["id"].as<int>();
        txn.commit();
        admins.push_back(new Admin(newId, username, pwd, email, phone, permLevel));
        if (newId >= nextUserID) nextUserID = newId + 1;
        return true;
    } catch (const exception& e) {
        cerr << "✗ DB registerAdmin: " << e.what() << endl;
        return false;
    }
}
```

- [ ] **Step 6: Update deleteAccount to write to DB**

Find `deleteAccount` in `AccountManager.cpp`. After the in-memory removal succeeds, add a DB delete before returning `true`:

```cpp
// Inside deleteAccount, after erasing from the relevant vector, before return true:
try {
    pqxx::work txn(Database::get());
    txn.exec_params("DELETE FROM users WHERE username = $1", username);
    txn.commit();
} catch (const exception& e) {
    cerr << "✗ DB deleteAccount: " << e.what() << endl;
}
return true;
```

- [ ] **Step 7: Update changeUserPassword to write to DB**

Find `changeUserPassword` in `AccountManager.cpp`. After successfully updating the in-memory password field, add:

```cpp
try {
    pqxx::work txn(Database::get());
    txn.exec_params("UPDATE users SET password_hash=$1 WHERE username=$2",
                    hashPassword(newPassword), username);
    txn.commit();
} catch (const exception& e) {
    cerr << "✗ DB changePassword: " << e.what() << endl;
}
```

- [ ] **Step 8: Commit**

```bash
git add backend/src/AccountManager.cpp
git commit -m "feat: AccountManager — write all mutations to PostgreSQL"
```

---

## Task 9: System — loadFromDB

**Files:** Modify `backend/include/System.h`, `backend/src/System.cpp`

- [ ] **Step 1: Add loadFromDB declaration to System.h**

Add in the public section alongside `loadFromFile`:

```cpp
bool loadFromDB();
```

- [ ] **Step 2: Add includes to System.cpp**

At the top of `System.cpp`, add:

```cpp
#include "Database.h"
#include <pqxx/pqxx>
```

- [ ] **Step 3: Add loadFromDB implementation to System.cpp**

```cpp
bool System::loadFromDB() {
    try {
        pqxx::work txn(Database::get());

        // Load stations
        pqxx::result stRows = txn.exec("SELECT id, name FROM stations ORDER BY id");
        for (const auto& sRow : stRows) {
            int    stId   = sRow["id"].as<int>();
            string stName = sRow["name"].c_str();
            Station* st   = new Station(stName, stId);

            // Load destinations
            pqxx::result dRows = txn.exec_params(
                "SELECT id, name FROM destinations WHERE station_id=$1 ORDER BY id", stId);
            for (const auto& dRow : dRows) {
                int    destId   = dRow["id"].as<int>();
                string destName = dRow["name"].c_str();
                Destination* dest = new Destination(destName);

                // Load louages
                pqxx::result lRows = txn.exec_params(
                    "SELECT serie_vehicule, numero_louage, id_prop "
                    "FROM louages WHERE destination_id=$1", destId);
                for (const auto& lRow : lRows) {
                    dest->ajouterLouage(Louage(
                        lRow["serie_vehicule"].as<int>(),
                        lRow["numero_louage"].as<int>(),
                        lRow["id_prop"].as<int>(),
                        destName, stName));
                }

                // Load reservations
                pqxx::result rRows = txn.exec_params(
                    "SELECT reservation_number, user_id, passenger_name, is_paid, times_called "
                    "FROM reservations WHERE destination_id=$1 ORDER BY reservation_number", destId);
                for (const auto& rRow : rRows) {
                    Reservation* res = new Reservation(
                        rRow["reservation_number"].as<int>(),
                        rRow["user_id"].as<int>(),
                        rRow["passenger_name"].c_str(),
                        destName, stName,
                        rRow["is_paid"].as<bool>(),
                        rRow["times_called"].as<int>());
                    dest->getReservationSystem()->addToQueue(res);
                }

                st->ajouterDestination(dest);
            }
            stations.push_back(st);
        }

        txn.commit();

        // Set reservation counter to max reservation_number + 1
        pqxx::work txn2(Database::get());
        pqxx::result maxRow = txn2.exec("SELECT COALESCE(MAX(reservation_number),0)+1 AS next FROM reservations");
        txn2.commit();
        Reservation::resetCounter(maxRow[0]["next"].as<int>());

        cout << "✓ " << stations.size() << " station(s) chargée(s) depuis PostgreSQL" << endl;
        return true;
    } catch (const exception& e) {
        cerr << "✗ Erreur loadFromDB (system): " << e.what() << endl;
        return false;
    }
}
```

- [ ] **Step 4: Commit**

```bash
git add backend/include/System.h backend/src/System.cpp
git commit -m "feat: System::loadFromDB — load stations/louages/reservations from PostgreSQL"
```

---

## Task 10: Reservation DB writes in web_server_main.cpp

**Files:** Modify `backend/src/web_server_main.cpp`

Add `#include "Database.h"` and `#include <pqxx/pqxx>` at the top alongside the other includes.

- [ ] **Step 1: Add DB includes at top of web_server_main.cpp**

After the existing `#include "html_templates.h"` line, add:

```cpp
#include "Database.h"
#include <pqxx/pqxx>
```

- [ ] **Step 2: Add helper — look up destination_id**

Add this helper function above `handleAPIReserve`:

```cpp
static int getDestinationId(const string& stationName, const string& destName) {
    try {
        pqxx::work txn(Database::get());
        pqxx::result r = txn.exec_params(
            "SELECT d.id FROM destinations d "
            "JOIN stations s ON s.id = d.station_id "
            "WHERE s.name=$1 AND d.name=$2",
            stationName, destName);
        txn.commit();
        if (r.empty()) return -1;
        return r[0]["id"].as<int>();
    } catch (...) { return -1; }
}
```

- [ ] **Step 3: Add DB write in handleAPIReserve**

Inside `handleAPIReserve`, after the `if (reservationNumber > 0)` block succeeds, add the DB write before building the JSON response:

```cpp
if (reservationNumber > 0) {
    // --- DB write ---
    int destId = getDestinationId(from, to);
    if (destId > 0) {
        try {
            pqxx::work txn(Database::get());
            txn.exec_params(
                "INSERT INTO reservations "
                "(reservation_number, user_id, passenger_name, destination_id, is_paid, times_called) "
                "VALUES ($1, $2, $3, $4, false, 0)",
                reservationNumber,
                session->username,   // store username as user_id reference
                name,
                destId);
            txn.commit();
        } catch (const exception& e) {
            cerr << "✗ DB reserve: " << e.what() << endl;
        }
    }
    // --- end DB write ---
    stringstream json;
    json << "{\"success\":true,\"reservationNumber\":" << reservationNumber << "}";
    // ... rest of existing response
```

Note: `user_id` column is INT but we are storing the username. Change the DB write to use `0` for user_id (anonymous) and keep the name field, OR add a sub-select to get the user id:

```cpp
txn.exec_params(
    "INSERT INTO reservations "
    "(reservation_number, user_id, passenger_name, destination_id, is_paid, times_called) "
    "VALUES ($1, COALESCE((SELECT id FROM users WHERE username=$2),0), $3, $4, false, 0)",
    reservationNumber, session->username, name, destId);
```

- [ ] **Step 4: Add DB write in handleAPIPayReservation**

Inside `handleAPIPayReservation`, after `confirmerPaiement` returns true, add:

```cpp
try {
    pqxx::work txn(Database::get());
    txn.exec_params(
        "UPDATE reservations SET is_paid=true WHERE reservation_number=$1", resNumber);
    txn.commit();
} catch (const exception& e) {
    cerr << "✗ DB pay: " << e.what() << endl;
}
```

- [ ] **Step 5: Add DB write in handleAPICancelReservation**

Inside `handleAPICancelReservation`, after `removeReservation` returns true, add:

```cpp
try {
    pqxx::work txn(Database::get());
    txn.exec_params(
        "DELETE FROM reservations WHERE reservation_number=$1", resNumber);
    txn.commit();
} catch (const exception& e) {
    cerr << "✗ DB cancel: " << e.what() << endl;
}
```

- [ ] **Step 6: Commit**

```bash
git add backend/src/web_server_main.cpp
git commit -m "feat: write reservation mutations (create/pay/cancel) to PostgreSQL"
```

---

## Task 11: Fix remaining bugs in web_server_main.cpp

**Files:** Modify `backend/src/web_server_main.cpp`

- [ ] **Step 1: Fix session expiry bug**

Find this line in `getAuthenticatedSession`:
```cpp
if (difftime(it->second.lastActivity, it->second.createdAt) > 1800) {
```
Replace with:
```cpp
if (difftime(time(nullptr), it->second.lastActivity) > 1800) {
```

- [ ] **Step 2: Fix recv buffer size**

Find:
```cpp
char buffer[4096] = {0};
int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
```
Replace with:
```cpp
char buffer[16384] = {0};
int bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
```

- [ ] **Step 3: Update main() to use loadFromDB**

Find in `main()`:
```cpp
louageSystem.loadFromFile("data/system_data.txt");
accountManager.loadFromFile("data/accounts.txt");
```
Replace with:
```cpp
try {
    Database::get(); // initialise connection — throws if DATABASE_URL missing
} catch (const exception& e) {
    cerr << "✗ Impossible de connecter à PostgreSQL: " << e.what() << endl;
    return 1;
}
louageSystem.loadFromDB();
accountManager.loadFromDB();
```

- [ ] **Step 4: Commit**

```bash
git add backend/src/web_server_main.cpp
git commit -m "fix: session expiry, recv buffer, startup uses PostgreSQL"
```

---

## Task 12: Build and test with Docker Compose

- [ ] **Step 1: Compute the actual SHA-256 of "stored" and patch schema.sql**

Run (Git Bash, WSL, or any Linux shell):
```bash
HASH=$(echo -n "stored" | openssl dgst -sha256 | awk '{print $2}')
echo $HASH
```

Open `sql/schema.sql` and replace every occurrence of `<HASH>` with the output.

Commit:
```bash
git add sql/schema.sql
git commit -m "chore: populate seed password hash in schema.sql"
```

- [ ] **Step 2: Build with Docker Compose**

```bash
cd "M:/LENOVO/Desktop/Projects/louage finder/LOUAGE_TRACKER"
docker compose up --build
```

Expected output includes:
```
✓ Connexion PostgreSQL établie
✓ 10 station(s) chargée(s) depuis PostgreSQL
✓ 9 compte(s) chargé(s) depuis PostgreSQL
Serveur demarre sur le port 8080...
```

If build errors appear, check `docker compose logs server`.

- [ ] **Step 3: Test homepage**

```bash
curl -s http://localhost:8080 | head -5
```

Expected: `<!DOCTYPE html>`

- [ ] **Step 4: Test login**

```bash
curl -s -c cookies.txt -d "username=admin&password=stored" http://localhost:8080/login -L | grep -i "dashboard\|erreur\|error"
```

Expected: redirects to `/dashboard` (response contains dashboard HTML).

- [ ] **Step 5: Test signup (new passenger)**

```bash
curl -s -c cookies2.txt \
  -d "role=passenger&username=testuser&email=test@test.com&phone=12345678&password=test123" \
  http://localhost:8080/signup -L | grep -i "login\|success"
```

Expected: redirects to `/login?success=...`

- [ ] **Step 6: Verify user persisted in DB**

```bash
docker compose exec db psql -U louage_user -d louage_tracker \
  -c "SELECT id, username, role FROM users WHERE username='testuser';"
```

Expected: one row with `testuser`.

- [ ] **Step 7: Test reservation then verify in DB**

Login as `ahmed`, make a reservation via the UI at `http://localhost:8080`, then:

```bash
docker compose exec db psql -U louage_user -d louage_tracker \
  -c "SELECT reservation_number, passenger_name, is_paid FROM reservations LIMIT 5;"
```

Expected: reservation row present.

- [ ] **Step 8: Commit passing test evidence**

```bash
git add .
git commit -m "test: docker compose build passes, login and reservation persist to PostgreSQL"
```

---

## Task 13: Push to GitHub

- [ ] **Step 1: Verify GitHub repo exists**

Open a browser and confirm `https://github.com/hajjour1230/lougetrackerv2` exists (or the correct casing of your username). If the repo doesn't exist yet, create it at `https://github.com/new` — name it `lougetrackerv2`, set it to **public** (so Render can access it), and do **not** initialise with a README.

- [ ] **Step 2: Add remote**

```bash
git remote add origin https://github.com/hajjour1230/lougetrackerv2.git
```

- [ ] **Step 3: Push**

```bash
git branch -M main
git push -u origin main
```

Expected: all commits pushed, no errors.

- [ ] **Step 4: Verify on GitHub**

Open `https://github.com/hajjour1230/lougetrackerv2` and confirm all files are present (`backend/`, `sql/`, `docker-compose.yml`, etc.).

---

## Task 14: Render deployment setup (reference)

These steps are done in the Render dashboard, not in code.

- [ ] Create a new **PostgreSQL** service on Render (free tier). Copy the **Internal Database URL**.
- [ ] Run schema once: from your local machine using the **External Connection URL**:
  ```bash
  psql "postgres://..." -f sql/schema.sql
  ```
- [ ] Create a new **Web Service** on Render, connect to `lougetrackerv2`, set **Root Directory** to `backend`.
- [ ] Add environment variable `DATABASE_URL` = the **Internal Database URL** from step 1.
- [ ] Deploy. Render will build the Docker image and start the server.
