#ifndef DESTINATION_H
#define DESTINATION_H

#include <vector>
#include <string>
#include "Louage.h"
#include "ReservationSystem.h"

class Destination {
private:
    std::string nom;
    std::vector<Louage> louages;
    ReservationSystem* reservationSystem;    
public:
    Destination(std::string n);
    ~Destination();
    
    void ajouterLouage(const Louage &l);
    void afficherLouages() const;
    std::string getNom() const;
    
    ReservationSystem* getReservationSystem();
    
    void louageArrive(Louage& louage);
    void louagePart();
    bool confirmerPaiement(int numeroReservation);
    
    int getTailleFileAttente() const;
    int getPlacesRestantes() const;
    
    vector<Louage>& getLouages();
    const vector<Louage>& getLouages() const;
    
    const ReservationSystem* getReservationSystem() const ;
    
    void afficherEtat() const;
};

#endif