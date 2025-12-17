#ifndef RESERVATIONSYSTEM_H
#define RESERVATIONSYSTEM_H

#include "ReservationQueue.h"
#include "Reservation.h"
#include "Louage.h"
#include <vector>
#include <string>
#include <chrono>

using namespace std;

class ReservationSystem {
private:
    ReservationQueue* fileAttente;    
    vector<Reservation*> appeles;
    vector<Reservation*> embarques;
    vector<Reservation*> partis;
    
    const int LOUAGE_CAPACITY = 8;
    const int TEMPS_ENTRE_APPELS = 20;
    
    chrono::system_clock::time_point dernierAppel;
    bool louageParti;
    bool louagePleinForce;

    Louage* currentLouage = nullptr;
    void preparerDepart();

public:
    ReservationSystem(ReservationQueue* queue);
    
    void demarrerRemplissage();
    void lancerVagueAppels();
    void marquerLouageParti();
    ReservationQueue demarrerRemplissageEtRetourner();
    
    bool confirmerPaiement(int reservationNumber);
    
    bool estPlein() const;
    bool estParti() const;
    bool peutLancerVague() const;
    
    vector<Reservation*> getAppeles() const;
    vector<Reservation*> getEmbarques() const;
    vector<Reservation*> getPartis() const;
    int getPlacesRestantes() const;
    int getPlacesOccupees() const;
    
    ReservationQueue* getQueue() const;  
    void addToQueue(Reservation* r) { fileAttente->addReservation(r); } 
    void setCurrentLouage(Louage* l) { currentLouage = l; }           
    Louage* getCurrentLouage() const { return currentLouage; }
    void resetSystem() {                                                    
        appeles.clear();
        embarques.clear();
        partis.clear();
        louageParti = false;
        louagePleinForce = false;
        dernierAppel = chrono::system_clock::now() - chrono::seconds(TEMPS_ENTRE_APPELS + 1);
    }

    // === UTILITAIRES ===
    void forcerAppel();
    void afficherEtat() const;

    ~ReservationSystem();
};

#endif
