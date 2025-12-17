#include "../include/Destination.h"
#include <iostream>

Destination::Destination(std::string n) : nom(n) {
    reservationSystem = new ReservationSystem(new ReservationQueue());
}

Destination::~Destination() {
    delete reservationSystem;
}

void Destination::ajouterLouage(const Louage &l) {
    louages.push_back(l);
}

void Destination::afficherLouages() const {
    cout << "  Louages disponibles: " << louages.size() << endl;
    for (size_t i = 0; i < louages.size(); i++) {
        cout << "    " << (i+1) << ". Louage #" << louages[i].getnumero_louage() 
             << " -> " << louages[i].getDestination() << endl;
    }
}

std::string Destination::getNom() const {
    return nom;
}

ReservationSystem* Destination::getReservationSystem() {
    return reservationSystem;
}

const ReservationSystem* Destination::getReservationSystem() const {
    return reservationSystem;
}

void Destination::louageArrive(Louage& louage) {
    reservationSystem->setCurrentLouage(&louage);
    reservationSystem->demarrerRemplissage();
}

void Destination::louagePart() {
    reservationSystem->marquerLouageParti();
    reservationSystem->resetSystem();
}

bool Destination::confirmerPaiement(int numeroReservation) {
    return reservationSystem->confirmerPaiement(numeroReservation);
}

int Destination::getTailleFileAttente() const {
    if (reservationSystem == nullptr) return 0;
    if (reservationSystem->getQueue() == nullptr) return 0;
    return reservationSystem->getQueue()->size();
}

int Destination::getPlacesRestantes() const {
    return reservationSystem->getPlacesRestantes();
}

void Destination::afficherEtat() const {
    std::cout << "=== Destination: " << nom << " ===" << std::endl;
    std::cout << "Louages disponibles: " << louages.size() << std::endl;
    reservationSystem->afficherEtat();
}

vector<Louage>& Destination::getLouages() {
    return louages;
}

const vector<Louage>& Destination::getLouages() const {
    return louages;
}

