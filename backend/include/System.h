#ifndef SYSTEM_H
#define SYSTEM_H

#include <vector>
#include <string>
#include <iostream>
#include "Louage.h"
#include "Destination.h"
#include "Station.h"

using namespace std;

class System {
private:
    vector<Station*> stations;  // CHANGED: Store pointers

public:
    System() {}
    ~System();  // ADDED: Destructor
    
    void ajouter_station(Station* s);  // CHANGED: Takes pointer
    
    Station* findStation(const string& name);
    const Station* findStation(const string& name) const;
    
    void afficher_stations() const;

    void ajouter_destination(const string& stationName, const Destination& dest);
    
    Destination* findDestination(const string& destName, const string& stationName);
    const Destination* findDestination(const string& destName, const string& stationName) const;

    void ajouter_louage(const string& stationName, const string& destName, const Louage& l);
    Louage* findLouage(const Louage& l, const string& destName, const string& stationName);
    void remove_louage(const string& stationName, const string& destName, const Louage& l);

    void saveToFile(const string& filename);
    void loadFromFile(const string& filename);

    vector<string> getAllStationNames() const;
    vector<string> getDestinationsFromStation(const string& stationName) const;
    vector<Louage> searchLouages(const string& fromStation, const string& toDestination);
};

#endif