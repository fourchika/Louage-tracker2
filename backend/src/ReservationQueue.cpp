#include "../include/ReservationQueue.h"
#include <algorithm>
#include "../include/Reservation.h"
#include <iostream>
#include <sstream>

void ReservationQueue::addReservation(Reservation* res) {
    reservationsQueue.push(res);
}

Reservation* ReservationQueue::getNextReservation() {
    if (reservationsQueue.empty()) {
        return nullptr;
    }
    
    Reservation* res = reservationsQueue.front();
    reservationsQueue.pop();
    return res;
}

Reservation* ReservationQueue::peekNextReservation() const {
    if (reservationsQueue.empty()) {
        return nullptr;
    }
    return reservationsQueue.front();
}

bool ReservationQueue::isEmpty() const {
    return reservationsQueue.empty();
}

int ReservationQueue::size() const {
    return (int)reservationsQueue.size();
}

void ReservationQueue::clear() {
    while (!reservationsQueue.empty()) {
        delete reservationsQueue.front();
        reservationsQueue.pop();
    }
}

Reservation* ReservationQueue::findReservation(int reservationNumber) const {
    std::queue<Reservation*> tempQueue = reservationsQueue;
    
    while (!tempQueue.empty()) {
        Reservation* res = tempQueue.front();
        if (res->getNumber() == reservationNumber) {
            return res;
        }
        tempQueue.pop();
    }
    return nullptr;
}

bool ReservationQueue::removeReservation(int reservationNumber) {
    bool found = false;
    std::queue<Reservation*> newQueue;
    
    while (!reservationsQueue.empty()) {
        Reservation* res = reservationsQueue.front();
        reservationsQueue.pop();
        
        if (res->getNumber() == reservationNumber) {
            delete res;
            found = true;
        } else {
            newQueue.push(res);
        }
    }
    
    reservationsQueue = newQueue;
    return found;
}

std::vector<Reservation*> ReservationQueue::getAllReservations() const {
    std::vector<Reservation*> all;
    std::queue<Reservation*> temp = reservationsQueue;
    
    while (!temp.empty()) {
        all.push_back(temp.front());
        temp.pop();
    }
    
    return all;
}

bool ReservationQueue::loadFromString(const string& data) {
    clear();
    
    if (data.empty()) {
        return true;
    }
    
    stringstream ss(data);
    string line;
    int count = 0;
    
    while (getline(ss, line)) {
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }
        
        if (line.empty()) continue;
        
        if (line[0] == '#') continue;
        
        if (line.find("Reservation|") != 0) {
            continue;
        }
        
        vector<string> fields;
        stringstream lineStream(line);
        string field;
        
        while (getline(lineStream, field, '|')) {
            fields.push_back(field);
        }
        
        if (fields.size() != 8) {
            cout << "  ⚠️ Format incorrect: " << fields.size() << " champs" << endl;
            continue;
        }
        
        try {
            int num         = stoi(fields[1]);
            int userId      = stoi(fields[2]);
            string name     = fields[3];
            string dest     = fields[4];
            string station  = fields[5];
            bool paid       = (fields[6] == "true" || fields[6] == "1");
            int timesCalled = stoi(fields[7]);

            Reservation* res = new Reservation(num, userId, name, dest, station, paid, timesCalled);
            addReservation(res);
            count++;

        } catch (const exception& e) {
            cout << "  ❌ Erreur parsing: " << e.what() << endl;
        }
    }
    
    if (count > 0) {
        cout << "    " << count << " réservation(s) chargée(s)" << endl;
    }
    
    return count > 0;
}