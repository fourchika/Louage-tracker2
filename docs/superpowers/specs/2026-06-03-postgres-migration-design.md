# Design: PostgreSQL Migration + Bug Fixes — Louage Tracker

**Date:** 2026-06-03  
**Status:** Approved

---

## Problem

The project currently stores all data in two text files (`data/system_data.txt`, `data/accounts.txt`). On Render, the container filesystem is ephemeral — every redeploy wipes runtime-written files. This causes all registrations, reservations, and payments to disappear. Additionally, several code bugs affect correctness locally and on Render.

---

## Goal

Replace text-file persistence with PostgreSQL. Fix all known bugs. Keep C++ and the existing in-memory architecture. Push result to GitHub (`lougetrackerv2`).

---

## Database Schema

Six tables. All `id` columns are `SERIAL PRIMARY KEY`.

```sql
users (
  id, username UNIQUE, password_hash, email UNIQUE, phone,
  role,              -- PASSENGER | DRIVER | CASHIER | MANAGER | ADMIN
  full_name,
  license_number,    -- DRIVER only
  experience_years,  -- DRIVER only
  station_name,      -- CASHIER only
  permission_level,  -- ADMIN only, default 3
  created_at
)

stations (
  id, name UNIQUE
)

destinations (
  id, station_id FK→stations, name,
  UNIQUE(station_id, name)
)

louages (
  id, destination_id FK→destinations,
  serie_vehicule, numero_louage, id_prop, occupied_seats DEFAULT 0
)

reservations (
  id, reservation_number UNIQUE,
  user_id, passenger_name,
  destination_id FK→destinations,
  is_paid DEFAULT false,
  times_called DEFAULT 0,
  created_at
)
```

`sql/schema.sql` creates all tables and seeds data equivalent to both current text files (10 stations, all routes, 9 user accounts with password `stored`).

---

## Architecture: Option A — In-Memory + DB Sync

Existing in-memory vectors (`vector<Station*>`, `vector<Passenger*>`, etc.) are kept. Changes:

- **Startup:** `loadFromDB()` replaces `loadFromFile()` — reads all rows from PostgreSQL into memory.
- **Mutations:** every write operation (register, make reservation, pay, cancel, delete account) writes to PostgreSQL immediately AND updates the in-memory state. No periodic saves.
- **No dual-write risk:** the HTTP server is single-threaded, so there are no concurrent write conflicts.

### New files

| File | Purpose |
|------|---------|
| `include/Database.h` | `Database` singleton: wraps `pqxx::connection`, reads `DATABASE_URL` env var |
| `src/Database.cpp` | Implementation; exposes `Database::get()` returning a `pqxx::connection&` |
| `sql/schema.sql` | DDL + seed data; run once on Render or locally |

### Changed files

| File | Change |
|------|--------|
| `AccountManager.h/.cpp` | Replace `loadFromFile`/`saveToFile` with `loadFromDB` + per-operation DB writes |
| `System.h/.cpp` | Replace `loadFromFile`/`saveToFile` with `loadFromDB` + per-operation DB writes |
| `web_server_main.cpp` | Fix session expiry bug; increase recv buffer 4 096 → 16 384 |
| `Dockerfile` | Add `libpqxx-dev libssl-dev`; fix compile flags (`-Isrc`, `-lpqxx -lpq -lssl -lcrypto`) |

---

## Bug Fixes

| # | Bug | Fix |
|---|-----|-----|
| 1 | `saveToFile` writes placeholder `email@example.com` and `hashPassword("stored")` for all users | Replaced by direct DB writes per operation |
| 2 | Session expiry compares `lastActivity` to `createdAt` but `lastActivity` is updated each request, so session never expires | Compare `lastActivity` to `time(nullptr)` |
| 3 | HTTP recv buffer is 4 096 bytes — large POST bodies (signup forms) get truncated | Increase to 16 384 |
| 4 | XOR password hashing (key `0x5A`) is trivially reversible | Replace with SHA-256 + per-user salt via OpenSSL; existing seed accounts get rehashed in schema.sql |
| 5 | `ReservationSystem.h` hardcoded absolute path | Already fixed (relative `"Louage.h"`) |
| 6 | `System.cpp` referenced `->queue` (wrong field name `reservationsQueue`) | Already fixed |
| 7 | Dockerfile missing `-Isrc` and `ws2_32` not needed on Linux | Fixed in updated Dockerfile |

---

## Environment Variables

| Variable | Description |
|----------|-------------|
| `DATABASE_URL` | Full PostgreSQL connection string, e.g. `postgresql://user:pass@host:5432/dbname` |

Locally: set in a `.env` file (not committed) or export in shell. On Render: set in the service environment dashboard and link to the Render Postgres instance.

---

## Deployment Flow

1. Create Render PostgreSQL instance → copy `DATABASE_URL`.
2. Run `sql/schema.sql` once against that instance (`psql $DATABASE_URL -f sql/schema.sql`).
3. Set `DATABASE_URL` env var on the Render web service.
4. Push to `lougetrackerv2` → Render auto-deploys.

---

## Out of Scope

- Multi-threading / connection pooling (single-threaded server, single `pqxx::connection`)
- Frontend changes
- Any new features
