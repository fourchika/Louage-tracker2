#ifndef CASHIER_H
#define CASHIER_H

#include "User.h"
#include "Reservation.h"
#include <vector>
#include <string>

using namespace std;

class Cashier : public User {
private:
    string stationName;
    vector<int> processedReservations;
    double totalCollected;
    int ticketsSold;

public:
    Cashier(int id, string username, string password, 
            string email, string phone, string station);
    
    bool login(string username, string password) override;
    bool changePassword(string oldPassword, string newPassword) override;
    void displayProfile() const override;
    
    bool processPayment(Reservation* reservation, double amount);
    bool issueTicket(int reservationNumber);
    bool cancelReservation(int reservationNumber);
    
    string getStationName() const;
    void setStationName(string station);
    
    int getTicketsSold() const;
    double getTotalCollected() const;
    void viewDailySummary() const;
    void resetDailyStats();
};

#endif
