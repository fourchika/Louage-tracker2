#include "../include/Louage.h"
#include <iostream>
using namespace std;

Louage::Louage(int sv, int num, int id, string des, string dep)
    : serie_vehicule(sv), numero_louage(num), id_prop(id), 
      destination(des), depart(dep), occupiedSeats(0) {
    
    cout << "🚌 Louage créé: #" << numero_louage 
         << " | " << depart << " → " << destination << endl;
}

void Louage::afficher() const {
    cout << "\n══════════ LOUAGE ══════════" << endl;
    cout << "Numéro: " << serie_vehicule << "-" << numero_louage << endl;
    cout << "Propriétaire: #" << id_prop << endl;
    cout << "Trajet: " << depart << " → " << destination << endl;
    cout << "Places occupées: " << occupiedSeats << "/8" << endl;
}

int Louage::getnumero_louage() const {
    return numero_louage;
}

int Louage::getSerieVehicule() const {
    return serie_vehicule;
}

int Louage::getIdProp() const {
    return id_prop;
}

string Louage::getDestination() const {
    return destination;
}

string Louage::getDepart() const {
    return depart;
}

int Louage::getOccupiedSeats() const {
    return occupiedSeats;
}

bool Louage::occupySeat() {
    if (occupiedSeats >= 8) {
        cout << "❌ Louage plein! (" << occupiedSeats << "/8)" << endl;
        return false;
    }
    
    occupiedSeats++;
    cout << "✅ Place occupée (" << occupiedSeats << "/8)" << endl;
    return true;
}