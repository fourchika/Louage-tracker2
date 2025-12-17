#include "../include/Driver.h"
#include <iostream>

using namespace std;

Driver::Driver(int id, string username, string password, 
               string email, string phone, string license, int experience)
    : User(id, username, password, email, phone), 
      licenseNumber(license), experienceYears(experience),
      assignedLouage(nullptr), totalEarnings(0.0), onDuty(false) {
        role="driver";
}

Driver::~Driver() {
}

bool Driver::login(string username, string password) {
    return User::login(username, password);
}

bool Driver::changePassword(string oldPassword, string newPassword) {
    return User::changePassword(oldPassword, newPassword);
}

void Driver::displayProfile() const {
    cout << "\n╔════════════════════════════════════╗" << endl;
    cout << "║        PROFIL CHAUFFEUR            ║" << endl;
    cout << "╚════════════════════════════════════╝" << endl;
    cout << "ID: " << getUserID() << endl;
    cout << "Nom d'utilisateur: " << getUsername() << endl;
    cout << "Rôle: " << getRole() << endl;
    cout << "Permis: " << licenseNumber << endl;
    cout << "Expérience: " << experienceYears << " ans" << endl;
    cout << "En service: " << (onDuty ? "Oui ✓" : "Non ✗") << endl;
    
    if (assignedLouage != nullptr) {
        cout << "\n🚌 Louage assigné: #" << assignedLouage->getnumero_louage() << endl;
        cout << "   Trajet: " << assignedLouage->getDepart() 
             << " → " << assignedLouage->getDestination() << endl;
    } else {
        cout << "\n⚠️ Aucun louage assigné" << endl;
    }
    
    cout << "\n📊 Statistiques:" << endl;
    cout << "   Total trajets: " << getTotalTrips() << endl;
    cout << "   Gains totaux: " << totalEarnings << " TND" << endl;
}

void Driver::assignLouage(Louage* louage) {
    if (assignedLouage != nullptr) {
        cout << "⚠️ Louage déjà assigné. Libérez-le d'abord." << endl;
        return;
    }
    
    assignedLouage = louage;
    cout << "✓ Louage #" << louage->getnumero_louage() << " assigné au chauffeur" << endl;
}

void Driver::unassignLouage() {
    if (assignedLouage == nullptr) {
        cout << "⚠️ Aucun louage à libérer" << endl;
        return;
    }
    
    assignedLouage = nullptr;
    cout << "✓ Louage libéré" << endl;
}

Louage* Driver::getAssignedLouage() const {
    return assignedLouage;
}

bool Driver::startTrip() {
    if (assignedLouage == nullptr) {
        cout << "✗ Aucun louage assigné" << endl;
        return false;
    }
    
    if (!onDuty) {
        cout << "✗ Vous n'êtes pas en service" << endl;
        return false;
    }
    
    cout << "✓ Trajet démarré: " << assignedLouage->getDepart() 
         << " → " << assignedLouage->getDestination() << endl;
    return true;
}

bool Driver::completeTrip(int passengers, double earnings) {
    if (assignedLouage == nullptr) {
        cout << "✗ Aucun trajet actif" << endl;
        return false;
    }
    
    string tripRecord = assignedLouage->getDepart() + " → " + 
                        assignedLouage->getDestination() + 
                        " (" + to_string(passengers) + " passagers)";
    
    completedTrips.push_back(tripRecord);
    totalEarnings += earnings;
    
    cout << "✓ Trajet terminé: " << tripRecord << endl;
    cout << "  Gains: " << earnings << " TND" << endl;
    
    return true;
}

void Driver::viewTripHistory() const {
    if (completedTrips.empty()) {
        cout << "Aucun trajet dans l'historique" << endl;
        return;
    }
    
    cout << "\n╔════════════════════════════════════╗" << endl;
    cout << "║     HISTORIQUE DES TRAJETS         ║" << endl;
    cout << "╚════════════════════════════════════╝" << endl;
    
    for (size_t i = 0; i < completedTrips.size(); i++) {
        cout << "[" << (i+1) << "] " << completedTrips[i] << endl;
    }
    
    cout << "\nTotal: " << completedTrips.size() << " trajets" << endl;
    cout << "Gains totaux: " << totalEarnings << " TND" << endl;
}

void Driver::setOnDuty(bool status) {
    onDuty = status;
    cout << (status ? "✓ En service" : "✓ Hors service") << endl;
}

bool Driver::isOnDuty() const {
    return onDuty;
}

int Driver::getTotalTrips() const {
    return completedTrips.size();
}

double Driver::getTotalEarnings() const {
    return totalEarnings;
}

string Driver::getLicenseNumber() const {
    return licenseNumber;
}

int Driver::getExperienceYears() const {
    return experienceYears;
}