#include "../include/Reservation.h"
#include <iostream>
#include <sstream>

using namespace std;

atomic<int> Reservation::nextNumber(1000);

Reservation::Reservation(int uid, string name, string dest, string stationName)
    : userId(uid), passengerName(name), destination(dest), 
      station(stationName), paid(false), timescalled(0) {
    number = nextNumber.fetch_add(1, memory_order_relaxed);
}

Reservation::Reservation(int num, int uid, string name, string dest, string stationName)
    : number(num), userId(uid), passengerName(name), destination(dest), 
      station(stationName), paid(false), timescalled(0) {
    int currentNext = nextNumber.load(memory_order_relaxed);
    if (num >= currentNext) {
        nextNumber.store(num + 1, memory_order_relaxed);
    }
}

Reservation::Reservation(int fixedNumber, int uid, string name, string dest, string stationName, bool isPaid, int times)
    : number(fixedNumber), userId(uid), passengerName(name), destination(dest), 
      station(stationName), paid(isPaid), timescalled(times) {
    
    // Mise à jour sécurisée du compteur global pour les futures nouvelles réservations
    int current = nextNumber.load(std::memory_order_relaxed);
    int desired = fixedNumber + 1;
    while (current < desired) {
        nextNumber.compare_exchange_weak(current, desired, std::memory_order_relaxed);
    }
}

void Reservation::resetCounter(int startFrom) {
    nextNumber.store(startFrom, memory_order_relaxed);
}

int Reservation::getNextNumber() {
    return nextNumber.load(memory_order_relaxed);
}

int Reservation::missedcall() {
    timescalled++;
    return timescalled;
}

int Reservation::getNumber() const { 
    return number; 
}

int Reservation::getUserId() const { 
    return userId; 
}

string Reservation::getPassengerName() const { 
    return passengerName; 
}

string Reservation::getDestination() const { 
    return destination; 
}

string Reservation::getStation() const { 
    return station; 
}

bool Reservation::isPaid() const { 
    return paid; 
}

// AJOUTE CES DEUX MÉTHODES MANQUANTES :
int Reservation::gettimescalled() const {
    return timescalled;
}

void Reservation::settimescalled(int times) {
    timescalled = times;
}

void Reservation::setPaid(bool isPaid) { 
    paid = isPaid; 
}

void Reservation::display() const {
    cout << "Reservation #" << number << endl;
    cout << "Passenger: " << passengerName << " (ID: " << userId << ")" << endl;
    cout << "From: " << station << " to " << destination << endl;
    cout << "Paid: " << (paid ? "Yes" : "No") << endl;
    cout << "Times called: " << timescalled << endl;
}

string Reservation::toString() const {
    stringstream ss;
    ss << "Reservation|" << number << "|" << userId << "|" << passengerName << "|"
       << destination << "|" << station << "|" << (paid ? "1" : "0") << "|"
       << timescalled;
    return ss.str();
}