-- Louage Tracker — PostgreSQL schema
-- Run once: psql $DATABASE_URL -f sql/schema.sql

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
-- Password for all seed accounts: "stored"

INSERT INTO users (id, username, password_hash, email, phone, role) VALUES
(1000, 'ahmed',           '87b04e58961f9a99d853d4046a0b5b793e7c3e4bbd21f5aca8fb17c20cdb1d8b', 'ahmed@example.com',      '98765432', 'PASSENGER'),
(1001, 'fatima',          '87b04e58961f9a99d853d4046a0b5b793e7c3e4bbd21f5aca8fb17c20cdb1d8b', 'fatima@example.com',     '97654321', 'PASSENGER'),
(1002, 'karim',           '87b04e58961f9a99d853d4046a0b5b793e7c3e4bbd21f5aca8fb17c20cdb1d8b', 'karim@example.com',      '96543210', 'PASSENGER'),
(1003, 'ali_driver',      '87b04e58961f9a99d853d4046a0b5b793e7c3e4bbd21f5aca8fb17c20cdb1d8b', 'ali@example.com',        '95432109', 'DRIVER'),
(1004, 'sami_driver',     '87b04e58961f9a99d853d4046a0b5b793e7c3e4bbd21f5aca8fb17c20cdb1d8b', 'sami@example.com',       '94321098', 'DRIVER'),
(1005, 'cashier_tunis',   '87b04e58961f9a99d853d4046a0b5b793e7c3e4bbd21f5aca8fb17c20cdb1d8b', 'cashier1@example.com',   '93210987', 'CASHIER'),
(1006, 'cashier_sousse',  '87b04e58961f9a99d853d4046a0b5b793e7c3e4bbd21f5aca8fb17c20cdb1d8b', 'cashier2@example.com',   '92109876', 'CASHIER'),
(1007, 'manager',         '87b04e58961f9a99d853d4046a0b5b793e7c3e4bbd21f5aca8fb17c20cdb1d8b', 'manager@example.com',    '91098765', 'MANAGER'),
(1008, 'admin',           '87b04e58961f9a99d853d4046a0b5b793e7c3e4bbd21f5aca8fb17c20cdb1d8b', 'admin@example.com',      '90987654', 'ADMIN')
ON CONFLICT DO NOTHING;

UPDATE users SET license_number='DRV001', experience_years=10 WHERE username='ali_driver';
UPDATE users SET license_number='DRV002', experience_years=5  WHERE username='sami_driver';
UPDATE users SET station_name='Tunis'  WHERE username='cashier_tunis';
UPDATE users SET station_name='Sousse' WHERE username='cashier_sousse';
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
(1,'Sousse'),(1,'Sfax'),(1,'Gabès'),(1,'Bizerte'),
(1,'Nabeul'),(1,'Kairouan'),(1,'Monastir'),(1,'Mahdia'),
(2,'Tunis'),(2,'Sfax'),(2,'Monastir'),(2,'Mahdia'),(2,'Kairouan'),(2,'Nabeul'),
(3,'Tunis'),(3,'Sousse'),(3,'Gabès'),(3,'Kairouan'),(3,'Mahdia'),
(4,'Tunis'),(4,'Sfax'),(4,'Medenine'),
(5,'Tunis'),(5,'Gabès'),(5,'Sfax'),
(8,'Tunis'),(8,'Sousse'),(8,'Sfax'),
(9,'Tunis'),(9,'Béja'),
(10,'Tunis'),(10,'Sousse'),
(11,'Tunis'),(11,'Sousse'),(11,'Mahdia'),
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
