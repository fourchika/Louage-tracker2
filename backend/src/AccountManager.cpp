#include "../include/AccountManager.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <openssl/sha.h>
#include "Database.h"
#include <pqxx/pqxx>

using namespace std;

AccountManager::AccountManager() : currentLoggedInUser(nullptr), nextUserID(1000) {}

AccountManager::~AccountManager() {
    // Clean up all user pointers
    for (auto* p : passengers) delete p;
    for (auto* d : drivers) delete d;
    for (auto* c : cashiers) delete c;
    for (auto* m : managers) delete m;
    for (auto* a : admins) delete a;
}

string AccountManager::hashPassword(const string& password) const {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256(reinterpret_cast<const unsigned char*>(password.c_str()),
           password.size(), hash);
    stringstream ss;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++)
        ss << hex << setw(2) << setfill('0') << static_cast<int>(hash[i]);
    return ss.str();
}

bool AccountManager::verifyPassword(const string& password, const string& hash) const {
    return hashPassword(password) == hash;
}

string AccountManager::trim(const string& str) const {
    size_t start = str.find_first_not_of(" \t\r\n");
    size_t end = str.find_last_not_of(" \t\r\n");
    if (start == string::npos) return "";
    return str.substr(start, end - start + 1);
}

// ============================================================
// REGISTRATION FUNCTIONS
// ============================================================

bool AccountManager::registerPassenger(string username, string password, string email, string phone, string fullName) {
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

bool AccountManager::registerDriver(string username, string password, string email, string phone, string license, int experience) {
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

bool AccountManager::registerCashier(string username, string password, string email, string phone, string station) {
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

bool AccountManager::registerManager(string username, string password, string email, string phone) {
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

bool AccountManager::registerAdmin(string username, string password, string email, string phone, int permLevel) {
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

// ============================================================
// LOGIN/LOGOUT
// ============================================================

User* AccountManager::login(string username, string password) {
    // Check passengers
    for (auto* p : passengers) {
        if (p->getUsername() == username) {
            // Debug: Show we found the user
            // cout << "DEBUG: Found user " << username << endl;
            if (p->login(username, password)) {
                currentLoggedInUser = p;
                cout << "✓ Connecté en tant que PASSAGER" << endl;
                return p;
            } else {
                cout << "✗ Mot de passe incorrect" << endl;
                return nullptr;
            }
        }
    }
    
    // Check drivers
    for (auto* d : drivers) {
        if (d->getUsername() == username) {
            if (d->login(username, password)) {
                currentLoggedInUser = d;
                cout << "✓ Connecté en tant que CHAUFFEUR" << endl;
                return d;
            } else {
                cout << "✗ Mot de passe incorrect" << endl;
                return nullptr;
            }
        }
    }
    
    // Check cashiers
    for (auto* c : cashiers) {
        if (c->getUsername() == username) {
            if (c->login(username, password)) {
                currentLoggedInUser = c;
                cout << "✓ Connecté en tant que CAISSIER" << endl;
                return c;
            } else {
                cout << "✗ Mot de passe incorrect" << endl;
                return nullptr;
            }
        }
    }
    
    // Check managers
    for (auto* m : managers) {
        if (m->getUsername() == username) {
            if (m->login(username, password)) {
                currentLoggedInUser = m;
                cout << "✓ Connecté en tant que MANAGER" << endl;
                return m;
            } else {
                cout << "✗ Mot de passe incorrect" << endl;
                return nullptr;
            }
        }
    }
    
    // Check admins
    for (auto* a : admins) {
        if (a->getUsername() == username) {
            if (a->login(username, password)) {
                currentLoggedInUser = a;
                cout << "✓ Connecté en tant que ADMIN" << endl;
                return a;
            } else {
                cout << "✗ Mot de passe incorrect" << endl;
                return nullptr;
            }
        }
    }
    
    cout << "✗ Nom d'utilisateur ou mot de passe incorrect" << endl;
    return nullptr;
}

void AccountManager::logout() {
    if (currentLoggedInUser) {
        cout << "✓ Déconnexion de " << currentLoggedInUser->getUsername() << endl;
        currentLoggedInUser = nullptr;
    }
}

User* AccountManager::getCurrentUser() const {
    return currentLoggedInUser;
}

bool AccountManager::isLoggedIn() const {
    return currentLoggedInUser != nullptr;
}

// ============================================================
// ACCOUNT MANAGEMENT
// ============================================================

bool AccountManager::usernameExists(const string& username) const {
    for (const auto* p : passengers) {
        if (p->getUsername() == username) return true;
    }
    for (const auto* d : drivers) {
        if (d->getUsername() == username) return true;
    }
    for (const auto* c : cashiers) {
        if (c->getUsername() == username) return true;
    }
    for (const auto* m : managers) {
        if (m->getUsername() == username) return true;
    }
    for (const auto* a : admins) {
        if (a->getUsername() == username) return true;
    }
    return false;
}

bool AccountManager::emailExists(const string& email) const {
    // Similar check for email - implement as needed
    return false;
}

User* AccountManager::findUserByUsername(const string& username) const {
    for (auto* p : passengers) {
        if (p->getUsername() == username) return p;
    }
    for (auto* d : drivers) {
        if (d->getUsername() == username) return d;
    }
    for (auto* c : cashiers) {
        if (c->getUsername() == username) return c;
    }
    for (auto* m : managers) {
        if (m->getUsername() == username) return m;
    }
    for (auto* a : admins) {
        if (a->getUsername() == username) return a;
    }
    return nullptr;
}

bool AccountManager::deleteAccount(const string& username) {
    // Search and erase from passengers
    for (auto it = passengers.begin(); it != passengers.end(); ++it) {
        if ((*it)->getUsername() == username) {
            delete *it;
            passengers.erase(it);
            try {
                pqxx::work txn(Database::get());
                txn.exec_params("DELETE FROM users WHERE username = $1", username);
                txn.commit();
            } catch (const exception& e) {
                cerr << "✗ DB deleteAccount: " << e.what() << endl;
            }
            return true;
        }
    }
    // Search and erase from drivers
    for (auto it = drivers.begin(); it != drivers.end(); ++it) {
        if ((*it)->getUsername() == username) {
            delete *it;
            drivers.erase(it);
            try {
                pqxx::work txn(Database::get());
                txn.exec_params("DELETE FROM users WHERE username = $1", username);
                txn.commit();
            } catch (const exception& e) {
                cerr << "✗ DB deleteAccount: " << e.what() << endl;
            }
            return true;
        }
    }
    // Search and erase from cashiers
    for (auto it = cashiers.begin(); it != cashiers.end(); ++it) {
        if ((*it)->getUsername() == username) {
            delete *it;
            cashiers.erase(it);
            try {
                pqxx::work txn(Database::get());
                txn.exec_params("DELETE FROM users WHERE username = $1", username);
                txn.commit();
            } catch (const exception& e) {
                cerr << "✗ DB deleteAccount: " << e.what() << endl;
            }
            return true;
        }
    }
    // Search and erase from managers
    for (auto it = managers.begin(); it != managers.end(); ++it) {
        if ((*it)->getUsername() == username) {
            delete *it;
            managers.erase(it);
            try {
                pqxx::work txn(Database::get());
                txn.exec_params("DELETE FROM users WHERE username = $1", username);
                txn.commit();
            } catch (const exception& e) {
                cerr << "✗ DB deleteAccount: " << e.what() << endl;
            }
            return true;
        }
    }
    // Search and erase from admins
    for (auto it = admins.begin(); it != admins.end(); ++it) {
        if ((*it)->getUsername() == username) {
            delete *it;
            admins.erase(it);
            try {
                pqxx::work txn(Database::get());
                txn.exec_params("DELETE FROM users WHERE username = $1", username);
                txn.commit();
            } catch (const exception& e) {
                cerr << "✗ DB deleteAccount: " << e.what() << endl;
            }
            return true;
        }
    }
    return false;
}

bool AccountManager::changeUserPassword(const string& username, const string& oldPassword, const string& newPassword) {
    User* user = findUserByUsername(username);
    if (!user) return false;
    // User::changePassword expects the stored hash as oldPwd
    string hashedNew = hashPassword(newPassword);
    if (!user->changePassword(hashPassword(oldPassword), hashedNew)) return false;
    try {
        pqxx::work txn(Database::get());
        txn.exec_params("UPDATE users SET password_hash=$1 WHERE username=$2",
                        hashedNew, username);
        txn.commit();
    } catch (const exception& e) {
        cerr << "✗ DB changePassword: " << e.what() << endl;
    }
    return true;
}

// ============================================================
// FILE OPERATIONS
// ============================================================

bool AccountManager::saveToFile(const string& filename) const {
    ofstream file(filename);
    if (!file.is_open()) {
        cout << "✗ Erreur: impossible d'ouvrir " << filename << endl;
        return false;
    }
    
    file << "# LOUAGE TRACKER - ACCOUNTS DATA" << endl;
    file << "# Format: Role|UserID|Username|PasswordHash|Email|Phone|ExtraData" << endl << endl;
    
    // Save passengers
    for (const auto* p : passengers) {
        file << "PASSENGER|" << p->getUserID() << "|" << p->getUsername() << "|"
             << hashPassword("stored") << "|" << "email@example.com" << "|" << "phone" << endl;
    }
    
    // Save drivers
    for (const auto* d : drivers) {
        file << "DRIVER|" << d->getUserID() << "|" << d->getUsername() << "|"
             << hashPassword("stored") << "|" << "email@example.com" << "|" << "phone" << "|"
             << d->getLicenseNumber() << "|" << d->getExperienceYears() << endl;
    }
    
    // Save cashiers
    for (const auto* c : cashiers) {
        file << "CASHIER|" << c->getUserID() << "|" << c->getUsername() << "|"
             << hashPassword("stored") << "|" << "email@example.com" << "|" << "phone" << "|"
             << c->getStationName() << endl;
    }
    
    // Save managers
    for (const auto* m : managers) {
        file << "MANAGER|" << m->getUserID() << "|" << m->getUsername() << "|"
             << hashPassword("stored") << "|" << "email@example.com" << "|" << "phone" << endl;
    }
    
    // Save admins
    for (const auto* a : admins) {
        file << "ADMIN|" << a->getUserID() << "|" << a->getUsername() << "|"
             << hashPassword("stored") << "|" << "email@example.com" << "|" << "phone" << "|"
             << a->getPermissionLevel() << endl;
    }
    
    file.close();
    cout << "✓ " << getTotalUsers() << " compte(s) sauvegardé(s) dans " << filename << endl;
    return true;
}

bool AccountManager::loadFromFile(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cout << "⚠️  Fichier " << filename << " non trouvé. Démarrage avec base vide." << endl;
        return false;
    }
    
    string line;
    int loaded = 0;
    
    while (getline(file, line)) {
        line = trim(line);
        
        if (line.empty() || line[0] == '#') continue;
        
        vector<string> parts;
        stringstream ss(line);
        string part;
        
        while (getline(ss, part, '|')) {
            parts.push_back(trim(part));
        }
        
        if (parts.size() < 6) continue;
        
        string role = parts[0];
        int userId = stoi(parts[1]);
        string username = parts[2];
        string passwordHash = parts[3];
        string email = parts[4];
        string phone = parts[5];
        
        // Update nextUserID
        if (userId >= nextUserID) {
            nextUserID = userId + 1;
        }
        
        if (role == "PASSENGER") {
            Passenger* p = new Passenger(userId, username, passwordHash, email, phone);
            passengers.push_back(p);
            loaded++;
        }
        else if (role == "DRIVER" && parts.size() >= 8) {
            string license = parts[6];
            int experience = stoi(parts[7]);
            Driver* d = new Driver(userId, username, passwordHash, email, phone, license, experience);
            drivers.push_back(d);
            loaded++;
        }
        else if (role == "CASHIER" && parts.size() >= 7) {
            string station = parts[6];
            Cashier* c = new Cashier(userId, username, passwordHash, email, phone, station);
            cashiers.push_back(c);
            loaded++;
        }
        else if (role == "MANAGER") {
            Manager* m = new Manager(userId, username, passwordHash, email, phone);
            managers.push_back(m);
            loaded++;
        }
        else if (role == "ADMIN" && parts.size() >= 7) {
            int permLevel = stoi(parts[6]);
            Admin* a = new Admin(userId, username, passwordHash, email, phone, permLevel);
            admins.push_back(a);
            loaded++;
        }
    }
    
    file.close();
    cout << "✓ " << loaded << " compte(s) chargé(s) depuis " << filename << endl;
    return true;
}

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

// ============================================================
// STATISTICS
// ============================================================

int AccountManager::getTotalUsers() const {
    return passengers.size() + drivers.size() + cashiers.size() + 
           managers.size() + admins.size();
}

int AccountManager::getUsersByRole(const string& role) const {
    if (role == "passenger") return passengers.size();
    if (role == "driver") return drivers.size();
    if (role == "cashier") return cashiers.size();
    if (role == "manager") return managers.size();
    if (role == "admin") return admins.size();
    return 0;
}

void AccountManager::displayAllUsers() const {
    cout << "\n╔════════════════════════════════════╗" << endl;
    cout << "║      TOUS LES COMPTES              ║" << endl;
    cout << "╚════════════════════════════════════╝" << endl;
    
    cout << "\n👥 PASSAGERS (" << passengers.size() << "):" << endl;
    for (const auto* p : passengers) {
        cout << "  ID: " << p->getUserID() << " | " << p->getUsername() << endl;
    }
    
    cout << "\n🚗 CHAUFFEURS (" << drivers.size() << "):" << endl;
    for (const auto* d : drivers) {
        cout << "  ID: " << d->getUserID() << " | " << d->getUsername() << endl;
    }
    
    cout << "\n💰 CAISSIERS (" << cashiers.size() << "):" << endl;
    for (const auto* c : cashiers) {
        cout << "  ID: " << c->getUserID() << " | " << c->getUsername() << endl;
    }
    
    cout << "\n📊 MANAGERS (" << managers.size() << "):" << endl;
    for (const auto* m : managers) {
        cout << "  ID: " << m->getUserID() << " | " << m->getUsername() << endl;
    }
    
    cout << "\n👑 ADMINS (" << admins.size() << "):" << endl;
    for (const auto* a : admins) {
        cout << "  ID: " << a->getUserID() << " | " << a->getUsername() << endl;
    }
    
    cout << "\nTotal: " << getTotalUsers() << " utilisateurs" << endl;
}

vector<Passenger*>& AccountManager::getPassengers() { return passengers; }
vector<Driver*>& AccountManager::getDrivers() { return drivers; }
vector<Cashier*>& AccountManager::getCashiers() { return cashiers; }
vector<Manager*>& AccountManager::getManagers() { return managers; }
vector<Admin*>& AccountManager::getAdmins() { return admins; }