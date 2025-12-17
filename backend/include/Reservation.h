#ifndef RESERVATION_H
#define RESERVATION_H

#include <string>
#include <atomic>
using namespace std;

class Reservation {
private:
    static atomic<int> nextNumber;
    int number;
    int userId;
    string passengerName;
    string destination;
    string station;       
    bool paid;       
    int timescalled; 

public:
    Reservation(int uid, string name, string dest, string stationName);
    Reservation(int num, int uid, string name, string dest, string stationName);
    Reservation(int fixedNumber, int uid, string name, string dest, string stationName, bool isPaid = false, int times = 0);
    
    static void resetCounter(int startFrom = 1);
    static int getNextNumber();
    int missedcall();
    
    int getNumber() const;
    int getUserId() const;
    string getPassengerName() const;
    string getDestination() const;
    string getStation() const;
    bool isPaid() const;
    
    // CORRIGE : Ajoute const ici
    int gettimescalled() const;
    
    void setPaid(bool isPaid);
    void settimescalled(int times);
    
    void display() const;
    string toString() const;
};

#endif