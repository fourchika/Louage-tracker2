#ifndef STATION_H
#define STATION_H

#include <vector>
#include <string>
#include "Destination.h"
using namespace std;

class Station {
private:
    string nom_station;
    int num_station;
    vector<Destination*> destinations;  // CHANGED: Now stores pointers

public:
    Station(string nom, int num);
    ~Station();  // ADDED: Destructor to clean up pointers
    
    // Disable copying to prevent double-free
    Station(const Station&) = delete;
    Station& operator=(const Station&) = delete;
    
    void ajouterDestination(Destination* des);  // CHANGED: Takes pointer
    void afficher() const;
    string getNom() const;
    vector<Destination*>& getDestinations();  // CHANGED: Returns pointer vector
    const vector<Destination*>& getDestinations() const;  // CHANGED: Returns pointer vector
    int getNumStation() const;
    
    int makeReservation(string destinationName, string passengerName, bool isDigital = true);
    Destination* findDestination(string name);
};

#endif