#include "../include/Passenger.h"
#include <iostream>

using namespace std;

Passenger::Passenger(int id, string username, string password, 
                     string email, string phone, string fullname)
    : User(id, username, password, email, phone), currentReservation(nullptr) {
    role = "passenger";
}

Passenger::~Passenger() {
    if (currentReservation != nullptr) {
        delete currentReservation;
    }
}

bool Passenger::login(string username, string password) {
    return User::login(username, password);
}

bool Passenger::changePassword(string oldPassword, string newPassword) {
    return User::changePassword(oldPassword, newPassword);
}

void Passenger::displayProfile() const {
    cout << "\n╔════════════════════════════════════╗" << endl;
    cout << "║        PROFIL PASSAGER             ║" << endl;
    cout << "╚════════════════════════════════════╝" << endl;
    cout << "ID: " << getUserID() << endl;
    cout << "Nom d'utilisateur: " << getUsername() << endl;
    cout << "Rôle: " << getRole() << endl;
    cout << "Total de trajets: " << getTotalTrips() << endl;
    cout << "Dépenses totales: " << getTotalSpent() << " TND" << endl;
    
    if (currentReservation != nullptr) {
        cout << "\n⚠️ Réservation active:" << endl;
        displayCurrentReservation();
    } else {
        cout << "\n✓ Aucune réservation active" << endl;
    }
}

bool Passenger::createReservation(string destination, string station) {
    if (currentReservation != nullptr) {
        cout << "✗ Vous avez déjà une réservation active" << endl;
        return false;
    }
    
    currentReservation = new Reservation(getUserID(), getUsername(), destination, station);
    cout << "✓ Réservation créée: #" << currentReservation->getNumber() << endl;
    return true;
}

bool Passenger::cancelCurrentReservation() {
    if (currentReservation == nullptr) {
        cout << "✗ Aucune réservation à annuler" << endl;
        return false;
    }
    
    delete currentReservation;
    currentReservation = nullptr;
    cout << "✓ Réservation annulée" << endl;
    return true;
}

bool Passenger::payCurrentReservation() {
    if (currentReservation == nullptr) {
        cout << "✗ Aucune réservation à payer" << endl;
        return false;
    }
    
    if (currentReservation->isPaid()) {
        cout << "⚠️ Réservation déjà payée" << endl;
        return false;
    }
    
    currentReservation->setPaid(true);
    cout << "✓ Paiement effectué pour la réservation #" << currentReservation->getNumber() << endl;
    return true;
}

bool Passenger::completeTrip() {
    if (currentReservation == nullptr) {
        cout << "✗ Aucune réservation active" << endl;
        return false;
    }
    
    if (!currentReservation->isPaid()) {
        cout << "✗ Réservation non payée" << endl;
        return false;
    }
    
    tripHistory.push_back(*currentReservation);
    delete currentReservation;
    currentReservation = nullptr;
    
    cout << "✓ Trajet terminé et ajouté à l'historique" << endl;
    return true;
}

bool Passenger::hasActiveReservation() const {
    return currentReservation != nullptr;
}

Reservation* Passenger::getCurrentReservation() const {
    return currentReservation;
}

const vector<Reservation>& Passenger::getTripHistory() const {
    return tripHistory;
}

int Passenger::getTotalTrips() const {
    return tripHistory.size();
}

double Passenger::getTotalSpent() const {
    return tripHistory.size() * 10.0;
}

void Passenger::displayCurrentReservation() const {
    if (currentReservation == nullptr) {
        cout << "Aucune réservation active" << endl;
        return;
    }
    currentReservation->display();
}

void Passenger::displayTripHistory() const {
    if (tripHistory.empty()) {
        cout << "Aucun trajet dans l'historique" << endl;
        return;
    }
    
    cout << "\n╔════════════════════════════════════╗" << endl;
    cout << "║     HISTORIQUE DES TRAJETS         ║" << endl;
    cout << "╚════════════════════════════════════╝" << endl;
    
    for (size_t i = 0; i < tripHistory.size(); i++) {
        cout << "\n[" << (i+1) << "] ";
        tripHistory[i].display();
    }
}