#include "../include/ReservationSystem.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <algorithm>

using namespace std;
using namespace chrono;

ReservationSystem::ReservationSystem(ReservationQueue* queue) 
    : fileAttente(queue), louageParti(false), louagePleinForce(false), currentLouage(nullptr) {
    dernierAppel = system_clock::now() - seconds(TEMPS_ENTRE_APPELS);
}

void ReservationSystem::demarrerRemplissage() {
    cout << "DÉMARRAGE DU REMPLISSAGE DU LOUAGE" << endl;
    cout << "-------------------------------------" << endl;
    
    louageParti = false;
    louagePleinForce = false;
    
    while (!louageParti && !estPlein()) {
        
        if (peutLancerVague()) {
            lancerVagueAppels();
        }
        
        afficherEtat();
        
        if (estPlein()) {
            cout << "\nLOUAGE PLEIN ! (" << embarques.size() << "/8 passagers)" << endl;
            louagePleinForce = true;
            break;
        }
        
        this_thread::sleep_for(seconds(10));
    }
    
    if (louagePleinForce || louageParti) {
        preparerDepart();
    }
}

void ReservationSystem::lancerVagueAppels() {
    cout << "\nVAGUE D'APPELS (" 
         << duration_cast<seconds>(system_clock::now() - dernierAppel).count()/60 
         << " min écoulées)" << endl;
    
    int placesRestantes = LOUAGE_CAPACITY - embarques.size();
    
    if (placesRestantes <= 0) {
        cout << "   Aucun appel nécessaire (louage plein)" << endl;
        return;
    }
    
    for (int i = 0; i < placesRestantes && !fileAttente->isEmpty(); i++) {
        Reservation* res = fileAttente->getNextReservation();
        if (res) {
            appeles.push_back(res);
            
            res->settimescalled(res->gettimescalled() + 1);
            
            cout << "   Appel #" << res->getNumber() 
                 << " - " << res->getPassengerName() 
                 << " (Appelé " << res->gettimescalled() << " fois)" << endl;
        }
    }
    
    dernierAppel = system_clock::now();
    
    if (appeles.size() == 0) {
        cout << "   Aucun passager en attente" << endl;
    }
}

void ReservationSystem::marquerLouageParti() {
    cout << "\nSTATION MARQUE LE LOUAGE COMME PARTI" << endl;
    louageParti = true;
    preparerDepart();
}

bool ReservationSystem::confirmerPaiement(int reservationNumber)
{
    for (Reservation* r : appeles) {
        if (r->getNumber() == reservationNumber) {

            r->setPaid(true);
            embarques.push_back(r);

            if (currentLouage != nullptr) {
                currentLouage->occupySeat();
            }

            cout << "Paye et monte ! #" << reservationNumber 
                 << " - " << r->getPassengerName() << endl;

            return true;
        }
    }

    cout << "Pas trouve ou deja paye #" << reservationNumber << endl;
    return false;
}

void ReservationSystem::preparerDepart() {
    cout << "\nPRÉPARATION DU DÉPART" << endl;
    cout << "=======================" << endl;
    
    for (Reservation* res : embarques) {
        if (res->isPaid()) {
            partis.push_back(res);
            cout << "   Départ: #" << res->getNumber() 
                 << " - " << res->getPassengerName();
            
            if (res->isPaid()) {
                cout << " (Payé)" << endl;
            } else {
                cout << " (Non payé - NE PART PAS)" << endl;
                partis.pop_back();
            }
        }
    }
    
    for (Reservation* res : appeles) {
        if (!res->isPaid()) {
            fileAttente->addReservation(res);
            cout << "   Retour file: #" << res->getNumber() 
                 << " - " << res->getPassengerName() 
                 << " (Non payé)" << endl;
        }
    }
    
    appeles.clear();
    embarques.clear();
    
    cout << "\nLOUAGE PARTI AVEC " << partis.size() << " PASSAGERS PAYANTS" << endl;
    
    if (partis.size() < LOUAGE_CAPACITY) {
        cout << "   Louage parti non-plein (" << partis.size() << "/8)" << endl;
    }
}

bool ReservationSystem::estPlein() const {
    return embarques.size() >= LOUAGE_CAPACITY;
}

bool ReservationSystem::estParti() const {
    return louageParti;
}

bool ReservationSystem::peutLancerVague() const {
    auto maintenant = system_clock::now();
    auto duree = duration_cast<seconds>(maintenant - dernierAppel).count();
    return duree >= TEMPS_ENTRE_APPELS;
}

vector<Reservation*> ReservationSystem::getAppeles() const {
    return appeles;
}

vector<Reservation*> ReservationSystem::getEmbarques() const {
    return embarques;
}

vector<Reservation*> ReservationSystem::getPartis() const {
    return partis;
}

int ReservationSystem::getPlacesRestantes() const {
    return LOUAGE_CAPACITY - embarques.size();
}

int ReservationSystem::getPlacesOccupees() const {
    return embarques.size();
}

void ReservationSystem::afficherEtat() const {
    cout << "\nÉTAT ACTUEL: ";
    cout << "File: " << fileAttente->size() << " | ";
    cout << "Appelés: " << appeles.size() << " | ";
    cout << "Embarqués: " << embarques.size() << "/8 | ";
    
    auto maintenant = system_clock::now();
    auto duree = duration_cast<seconds>(maintenant - dernierAppel).count();
    int minutesRestantes = max(0, (TEMPS_ENTRE_APPELS - (int)duree) / 60);
    int secondesRestantes = (TEMPS_ENTRE_APPELS - (int)duree) % 60;
    
    cout << "Prochain appel: " << minutesRestantes << "m" << secondesRestantes << "s";
    
    if (louageParti) cout << " | PARTI";
    if (estPlein()) cout << " | PLEIN";
    
    cout << endl;
}

void ReservationSystem::forcerAppel() {
    if (!fileAttente->isEmpty() && getPlacesRestantes() > 0) {
        lancerVagueAppels();
    }
}

ReservationQueue ReservationSystem::demarrerRemplissageEtRetourner() {
    ReservationQueue nouvelleFile = *fileAttente;
    
    ReservationSystem systemeTemporaire(&nouvelleFile);
    systemeTemporaire.demarrerRemplissage();
    
    return nouvelleFile;
}
ReservationSystem::~ReservationSystem() {
    delete fileAttente;
}

ReservationQueue* ReservationSystem::getQueue() const {
    return fileAttente;
}

