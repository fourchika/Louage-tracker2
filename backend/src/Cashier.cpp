#include "../include/Cashier.h"
#include <iostream>

using namespace std;

Cashier::Cashier(int id, string username, string password, 
                 string email, string phone, string station)
    : User(id, username, password, email, phone), 
      stationName(station), totalCollected(0.0), ticketsSold(0) {
        role = "cashier";
}

bool Cashier::login(string username, string password) {
    return User::login(username, password);
}

bool Cashier::changePassword(string oldPassword, string newPassword) {
    return User::changePassword(oldPassword, newPassword);
}

void Cashier::displayProfile() const {
    cout << "\n╔════════════════════════════════════╗" << endl;
    cout << "║        PROFIL CAISSIER             ║" << endl;
    cout << "╚════════════════════════════════════╝" << endl;
    cout << "ID: " << getUserID() << endl;
    cout << "Nom d'utilisateur: " << getUsername() << endl;
    cout << "Rôle: " << getRole() << endl;
    cout << "Station: " << stationName << endl;
    
    cout << "\n📊 Statistiques du jour:" << endl;
    cout << "   Tickets vendus: " << ticketsSold << endl;
    cout << "   Montant collecté: " << totalCollected << " TND" << endl;
    cout << "   Réservations traitées: " << processedReservations.size() << endl;
}

bool Cashier::processPayment(Reservation* reservation, double amount) {
    if (reservation == nullptr) {
        cout << "✗ Réservation invalide" << endl;
        return false;
    }
    
    if (reservation->isPaid()) {
        cout << "⚠️ Réservation déjà payée: #" << reservation->getNumber() << endl;
        return false;
    }
    
    reservation->setPaid(true);
    totalCollected += amount;
    processedReservations.push_back(reservation->getNumber());
    
    cout << "✓ Paiement traité: " << amount << " TND" << endl;
    cout << "  Réservation #" << reservation->getNumber() << endl;
    
    return true;
}

bool Cashier::issueTicket(int reservationNumber) {
    bool found = false;
    for (int num : processedReservations) {
        if (num == reservationNumber) {
            found = true;
            break;
        }
    }
    
    if (!found) {
        cout << "✗ Réservation #" << reservationNumber << " non trouvée ou non payée" << endl;
        return false;
    }
    
    ticketsSold++;
    
    cout << "\n╔════════════════════════════════════╗" << endl;
    cout << "║           TICKET ÉMIS              ║" << endl;
    cout << "╚════════════════════════════════════╝" << endl;
    cout << "  Numéro: #" << reservationNumber << endl;
    cout << "  Station: " << stationName << endl;
    cout << "  Caissier: " << getUsername() << endl;
    cout << "════════════════════════════════════" << endl;
    
    return true;
}

bool Cashier::cancelReservation(int reservationNumber) {
    cout << "⚠️ Annulation de la réservation #" << reservationNumber << endl;
    cout << "  Remboursement à traiter" << endl;
    
    for (auto it = processedReservations.begin(); it != processedReservations.end(); ++it) {
        if (*it == reservationNumber) {
            processedReservations.erase(it);
            cout << "✓ Réservation annulée" << endl;
            return true;
        }
    }
    
    cout << "✗ Réservation non trouvée" << endl;
    return false;
}

string Cashier::getStationName() const {
    return stationName;
}

void Cashier::setStationName(string station) {
    stationName = station;
    cout << "✓ Station mise à jour: " << station << endl;
}

int Cashier::getTicketsSold() const {
    return ticketsSold;
}

double Cashier::getTotalCollected() const {
    return totalCollected;
}

void Cashier::viewDailySummary() const {
    cout << "\n╔════════════════════════════════════╗" << endl;
    cout << "║       RÉSUMÉ QUOTIDIEN             ║" << endl;
    cout << "╚════════════════════════════════════╝" << endl;
    cout << "Station: " << stationName << endl;
    cout << "Caissier: " << getUsername() << endl;
    cout << "\n📊 Statistiques:" << endl;
    cout << "   Tickets vendus: " << ticketsSold << endl;
    cout << "   Réservations traitées: " << processedReservations.size() << endl;
    cout << "   Montant total collecté: " << totalCollected << " TND" << endl;
    
    if (ticketsSold > 0) {
        cout << "   Prix moyen: " << (totalCollected / ticketsSold) << " TND" << endl;
    }
}

void Cashier::resetDailyStats() {
    ticketsSold = 0;
    totalCollected = 0.0;
    processedReservations.clear();
    cout << "✓ Statistiques quotidiennes réinitialisées" << endl;
}