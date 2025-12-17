#ifndef DRIVER_H
#define DRIVER_H

#include "User.h"
#include "Louage.h"
#include <vector>
#include <string>

using namespace std;

class Driver : public User {
private:
    string licenseNumber;
    int experienceYears;
    Louage* assignedLouage;
    vector<string> completedTrips;
    double totalEarnings;
    bool onDuty;

public:
    Driver(int id, string username, string password, 
           string email, string phone, string license, int experience);
    
    ~Driver();
    
    bool login(string username, string password) override;
    bool changePassword(string oldPassword, string newPassword) override;
    void displayProfile() const override;
    
    void assignLouage(Louage* louage);
    void unassignLouage();
    Louage* getAssignedLouage() const;
    
    bool startTrip();
    bool completeTrip(int passengers, double earnings);
    void viewTripHistory() const;
    
    void setOnDuty(bool status);
    bool isOnDuty() const;
    
    int getTotalTrips() const;
    double getTotalEarnings() const;
    string getLicenseNumber() const;
    int getExperienceYears() const;
};

#endif