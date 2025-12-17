#ifndef RESERVATIONQUEUE_H
#define RESERVATIONQUEUE_H

#include <queue>
#include "Reservation.h"
#include <vector>
#include <iostream> 
using namespace std; 

class ReservationQueue {
public:   
    queue<Reservation*> reservationsQueue;
    void addReservation(Reservation* res);
    Reservation* getNextReservation();
    Reservation* peekNextReservation() const;
    bool isEmpty() const;
    int size() const;
    void clear();
    
    Reservation* findReservation(int reservationNumber) const;
    bool removeReservation(int reservationNumber);
    vector<Reservation*> getAllReservations() const;
    bool loadFromString(const std::string& data);
};

#endif
