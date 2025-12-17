#ifndef PASSENGER_H
#define PASSENGER_H

#include "User.h"
#include "Reservation.h"
#include <vector>
#include <string>

using namespace std;

class Passenger : public User {
private:
    Reservation* currentReservation;   
    vector<Reservation> tripHistory;
    
public:
    Passenger(int id, string username, string password, 
              string email, string phone, string fullname = "");
    
    bool login(string username, string password) override;
    bool changePassword(string oldPassword, string newPassword) override;
    void displayProfile() const override;
    
    bool createReservation(string destination, string station);
    bool cancelCurrentReservation();
    bool payCurrentReservation();
    bool completeTrip();
    
    bool hasActiveReservation() const;
    Reservation* getCurrentReservation() const;
    const vector<Reservation>& getTripHistory() const;
    
    int getTotalTrips() const;
    double getTotalSpent() const;
    
    void displayCurrentReservation() const;
    void displayTripHistory() const;
    
    ~Passenger();
};

#endif