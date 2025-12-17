#include "../include/Station.h"
#include <iostream>

using namespace std;

Station::Station(string nom, int num) {
    nom_station = nom;
    num_station = num;
}

Station::~Station() {
    for (auto* dest : destinations) {
        delete dest;
    }
}

void Station::ajouterDestination(Destination* des) {
    destinations.push_back(des);
    cout << "  [+] Destination ajoutée à " << nom_station << ": " << des->getNom() << endl;
}

void Station::afficher() const {
    cout << "=== Station " << nom_station << " (N°" << num_station << ") ===" << endl;
    for (const auto* d : destinations) {
        cout << "  Destination: " << d->getNom() << endl;
        d->afficherLouages();
    }
}

string Station::getNom() const {
    return nom_station;
}

vector<Destination*>& Station::getDestinations() {
    return destinations; 
}

int Station::getNumStation() const {
    return num_station;
}

const vector<Destination*>& Station::getDestinations() const {
    return destinations;
}

int Station::makeReservation(string destinationName, string passengerName, bool isDigital) {
    for (auto* dest : destinations) {
        if (dest->getNom() == destinationName) {
            int userId = Reservation::getNextNumber();
            Reservation* res = new Reservation(userId, passengerName, destinationName, nom_station);
            dest->getReservationSystem()->addToQueue(res);
            return res->getNumber();
        }
    }
     
    cerr << "❌ ERREUR INTERNE: Destination '" << destinationName 
         << "' introuvable dans " << nom_station << endl;
    return -1;
}

Destination* Station::findDestination(string name) {
    for (auto* dest : destinations) {
        if (dest->getNom() == name) {
            return dest;
        }
    }
    return nullptr;
}