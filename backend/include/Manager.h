#ifndef MANAGER_H
#define MANAGER_H

#include "User.h"
#include "Station.h"
#include "Louage.h"
#include <vector>
#include <string>

using namespace std;

class Manager : public User {
private:
    vector<string> managedStations;
    vector<string> decisions;

public:
    Manager(int id, string username, string password, 
            string email, string phone);
    
    bool login(string username, string password) override;
    bool changePassword(string oldPassword, string newPassword) override;
    void displayProfile() const override;
    
    void addManagedStation(string stationName);
    void removeManagedStation(string stationName);
    vector<string> getManagedStations() const;
    
    bool approveLouageAssignment(int louageNumber, int driverID);
    bool adjustPricing(string route, double newPrice);
    bool scheduleRoute(string from, string to, string time);
    
    void viewStationReport(string stationName) const;
    void viewAllStationsReport() const;
    void recordDecision(string decision);
    void viewDecisionHistory() const;
};

#endif
