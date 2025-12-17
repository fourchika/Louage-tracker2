#include "../include/AccountManager.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>

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

// Simple hash function (in production, use bcrypt or similar)
string AccountManager::hashPassword(const string& password) const {
    // Simple XOR hash for demo - USE PROPER HASHING IN PRODUCTION!
    string hashed = password;
    for (size_t i = 0; i < hashed.length(); i++) {
        hashed[i] = hashed[i] ^ 0x5A; // XOR with key
    }
    return hashed;
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
    if (usernameExists(username)) {
        cout << "✗ Nom d'utilisateur déjà pris" << endl;
        return false;
    }
    
    if (emailExists(email)) {
        cout << "✗ Email déjà utilisé" << endl;
        return false;
    }
    
    string hashedPwd = hashPassword(password);
    Passenger* newPassenger = new Passenger(nextUserID++, username, hashedPwd, email, phone, fullName);
    passengers.push_back(newPassenger);
    
    cout << "✓ Compte passager créé avec succès!" << endl;
    cout << "  ID: " << newPassenger->getUserID() << endl;
    cout << "  Username: " << username << endl;
    return true;
}

bool AccountManager::registerDriver(string username, string password, string email, string phone, string license, int experience) {
    if (usernameExists(username)) {
        cout << "✗ Nom d'utilisateur déjà pris" << endl;
        return false;
    }
    
    if (emailExists(email)) {
        cout << "✗ Email déjà utilisé" << endl;
        return false;
    }
    
    string hashedPwd = hashPassword(password);
    Driver* newDriver = new Driver(nextUserID++, username, hashedPwd, email, phone, license, experience);
    drivers.push_back(newDriver);
    
    cout << "✓ Compte chauffeur créé avec succès!" << endl;
    return true;
}

bool AccountManager::registerCashier(string username, string password, string email, string phone, string station) {
    if (usernameExists(username)) {
        cout << "✗ Nom d'utilisateur déjà pris" << endl;
        return false;
    }
    
    if (emailExists(email)) {
        cout << "✗ Email déjà utilisé" << endl;
        return false;
    }
    
    string hashedPwd = hashPassword(password);
    Cashier* newCashier = new Cashier(nextUserID++, username, hashedPwd, email, phone, station);
    cashiers.push_back(newCashier);
    
    cout << "✓ Compte caissier créé avec succès!" << endl;
    return true;
}

bool AccountManager::registerManager(string username, string password, string email, string phone) {
    if (usernameExists(username)) {
        cout << "✗ Nom d'utilisateur déjà pris" << endl;
        return false;
    }
    
    if (emailExists(email)) {
        cout << "✗ Email déjà utilisé" << endl;
        return false;
    }
    
    string hashedPwd = hashPassword(password);
    Manager* newManager = new Manager(nextUserID++, username, hashedPwd, email, phone);
    managers.push_back(newManager);
    
    cout << "✓ Compte manager créé avec succès!" << endl;
    return true;
}

bool AccountManager::registerAdmin(string username, string password, string email, string phone, int permLevel) {
    if (usernameExists(username)) {
        cout << "✗ Nom d'utilisateur déjà pris" << endl;
        return false;
    }
    
    if (emailExists(email)) {
        cout << "✗ Email déjà utilisé" << endl;
        return false;
    }
    
    string hashedPwd = hashPassword(password);
    Admin* newAdmin = new Admin(nextUserID++, username, hashedPwd, email, phone, permLevel);
    admins.push_back(newAdmin);
    
    cout << "✓ Compte admin créé avec succès!" << endl;
    return true;
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