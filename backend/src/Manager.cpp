#include "../include/Manager.h"
#include <iostream>
#include <algorithm>

using namespace std;

Manager::Manager(int id, string username, string password, 
                 string email, string phone)
    : User(id, username, password, email, phone) {
        role="manager";
}

bool Manager::login(string username, string password) {
    return User::login(username, password);
}

bool Manager::changePassword(string oldPassword, string newPassword) {
    return User::changePassword(oldPassword, newPassword);
}

void Manager::displayProfile() const {
    cout << "\n╔════════════════════════════════════╗" << endl;
    cout << "║        PROFIL MANAGER              ║" << endl;
    cout << "╚════════════════════════════════════╝" << endl;
    cout << "ID: " << getUserID() << endl;
    cout << "Nom d'utilisateur: " << getUsername() << endl;
    cout << "Rôle: " << getRole() << endl;
    
    cout << "\n🏢 Stations gérées (" << managedStations.size() << "):" << endl;
    if (managedStations.empty()) {
        cout << "   Aucune station assignée" << endl;
    } else {
        for (size_t i = 0; i < managedStations.size(); i++) {
            cout << "   " << (i+1) << ". " << managedStations[i] << endl;
        }
    }
    
    cout << "\n📋 Décisions prises: " << decisions.size() << endl;
}

void Manager::addManagedStation(string stationName) {
    for (const auto& station : managedStations) {
        if (station == stationName) {
            cout << "⚠️ Station déjà sous votre gestion" << endl;
            return;
        }
    }
    
    managedStations.push_back(stationName);
    recordDecision("Ajout de la station: " + stationName);
    cout << "✓ Station ajoutée: " << stationName << endl;
}

void Manager::removeManagedStation(string stationName) {
    auto it = find(managedStations.begin(), managedStations.end(), stationName);
    if (it != managedStations.end()) {
        managedStations.erase(it);
        recordDecision("Retrait de la station: " + stationName);
        cout << "✓ Station retirée: " << stationName << endl;
    } else {
        cout << "✗ Station non trouvée dans votre gestion" << endl;
    }
}

vector<string> Manager::getManagedStations() const {
    return managedStations;
}

bool Manager::approveLouageAssignment(int louageNumber, int driverID) {
    cout << "✓ Attribution approuvée:" << endl;
    cout << "  Louage #" << louageNumber << " → Chauffeur ID: " << driverID << endl;
    
    recordDecision("Approuvé: Louage #" + to_string(louageNumber) + 
                   " pour chauffeur #" + to_string(driverID));
    return true;
}

bool Manager::adjustPricing(string route, double newPrice) {
    if (newPrice <= 0) {
        cout << "✗ Prix invalide" << endl;
        return false;
    }
    
    cout << "✓ Prix ajusté pour " << route << ": " << newPrice << " TND" << endl;
    recordDecision("Ajustement prix: " + route + " → " + to_string(newPrice) + " TND");
    return true;
}

bool Manager::scheduleRoute(string from, string to, string time) {
    cout << "✓ Route programmée:" << endl;
    cout << "  " << from << " → " << to << " à " << time << endl;
    
    recordDecision("Route programmée: " + from + " → " + to + " (" + time + ")");
    return true;
}

void Manager::viewStationReport(string stationName) const {
    cout << "\n╔════════════════════════════════════╗" << endl;
    cout << "║      RAPPORT DE STATION            ║" << endl;
    cout << "╚════════════════════════════════════╝" << endl;
    cout << "Station: " << stationName << endl;
    cout << "\n[Données en temps réel à intégrer]" << endl;
    cout << "- Louages actifs: N/A" << endl;
    cout << "- Réservations en attente: N/A" << endl;
    cout << "- Revenus du jour: N/A" << endl;
}

void Manager::viewAllStationsReport() const {
    cout << "\n╔════════════════════════════════════╗" << endl;
    cout << "║     RAPPORT GLOBAL STATIONS        ║" << endl;
    cout << "╚════════════════════════════════════╝" << endl;
    
    if (managedStations.empty()) {
        cout << "Aucune station à afficher" << endl;
        return;
    }
    
    for (const auto& station : managedStations) {
        cout << "\n📍 " << station << endl;
        cout << "   Status: Opérationnel" << endl;
        cout << "   Activité: Normale" << endl;
    }
}

void Manager::recordDecision(string decision) {
    decisions.push_back(decision);
}

void Manager::viewDecisionHistory() const {
    if (decisions.empty()) {
        cout << "Aucune décision enregistrée" << endl;
        return;
    }
    
    cout << "\n╔════════════════════════════════════╗" << endl;
    cout << "║    HISTORIQUE DES DÉCISIONS        ║" << endl;
    cout << "╚════════════════════════════════════╝" << endl;
    
    for (size_t i = 0; i < decisions.size(); i++) {
        cout << "[" << (i+1) << "] " << decisions[i] << endl;
    }
}