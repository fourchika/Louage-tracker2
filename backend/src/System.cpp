#include "../include/System.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>

using namespace std;

System::~System() {
    for (auto* station : stations) {
        delete station;
    }
}

string normalizeString(const string& str) {
    string normalized = str;
    size_t start = normalized.find_first_not_of(" \t\n\r");
    size_t end = normalized.find_last_not_of(" \t\n\r");
    if (start != string::npos && end != string::npos) {
        normalized = normalized.substr(start, end - start + 1);
    }
    transform(normalized.begin(), normalized.end(), normalized.begin(),
              [](unsigned char c) { return tolower(c); });
    return normalized;
}

void System::ajouter_station(Station* s) {
    if (findStation(s->getNom()) == nullptr) {
        stations.push_back(s);
    }
}

Station* System::findStation(const string& name) {
    string normalizedName = normalizeString(name);
    for (auto* s : stations) {
        if (normalizeString(s->getNom()) == normalizedName)
            return s;
    }
    return nullptr;
}

const Station* System::findStation(const string& name) const {
    string normalizedName = normalizeString(name);
    for (const auto* s : stations) {
        if (normalizeString(s->getNom()) == normalizedName)
            return s;
    }
    return nullptr;
}

void System::afficher_stations() const {
    int i = 1;
    for(const auto* l : stations) {
        cout << "-------" << i << "------" << l->getNom() << endl;
        i++;
    }
}

Destination* System::findDestination(const string& destName, const string& stationName) {
    Station* x = findStation(stationName);
    if (x == nullptr) {
        return nullptr;
    }
    else {
        string normalizedDestName = normalizeString(destName);
        for (auto* s : x->getDestinations()) {
            if (normalizeString(s->getNom()) == normalizedDestName)
                return s;
        }
        return nullptr;
    }
}

const Destination* System::findDestination(const string& destName, const string& stationName) const {
    const Station* x = findStation(stationName);
    if (x == nullptr) {
        return nullptr;
    }
    else {
        string normalizedDestName = normalizeString(destName);
        for (const auto* s : x->getDestinations()) {
            if (normalizeString(s->getNom()) == normalizedDestName)
                return s;
        }
        return nullptr;
    }
}

Louage* System::findLouage(const Louage& l, const string& destName, const string& stationName) {
    Destination* d = findDestination(destName, stationName);
    if (d == nullptr) {
        return nullptr;
    }

    for (auto& lou : d->getLouages()) {
        if (lou.getnumero_louage() == l.getnumero_louage()) {
            return &lou;
        }
    }

    return nullptr;
}

void System::ajouter_louage(const string& stationName, const string& destName, const Louage& l) {
    Station* s = findStation(stationName);
    if (s == nullptr) {
        cout << "Station " << stationName << " introuvable!" << endl;
        return;
    }

    Destination* d = findDestination(destName, stationName);
    if (d == nullptr) {
        cout << "destination " << destName << " introuvable!" << endl;
        return;
    }

    Louage* x = findLouage(l, destName, stationName);
    if(x != nullptr) {
        cout << " le louage numero" << l.getnumero_louage() << "est deja pret dans "
             << stationName << "vers la destination" << destName << endl;
    }
    else {
        vector<Louage>& Lous = d->getLouages();
        Lous.push_back(l);
    }
}

void System::remove_louage(const string& stationName, const string& destName, const Louage& l) {
    Station* s = findStation(stationName);
    if (s == nullptr) {
        cout << "Station " << stationName << " introuvable!" << endl;
        return;
    }

    Destination* d = findDestination(destName, stationName);
    if (d == nullptr) {
        cout << "destination " << destName << " introuvable!" << endl;
        return;
    }

    Louage* x = findLouage(l, destName, stationName);
    if(x == nullptr) {
        cout << " le louage numero" << l.getnumero_louage() << "n'est pas deja pret dans " << endl;
    }

    vector<Louage>& L = d->getLouages();
    bool removed = false;

    for (auto it = L.begin(); it != L.end(); it++) {
        if (it->getnumero_louage() == l.getnumero_louage()) {
            L.erase(it);
            removed = true;
            break;
        }
    }

    if (!removed) {
        cout << "Aucun louage avec le numéro " << l.getnumero_louage() << " n'existe." << endl;
    }
}

void System::saveToFile(const string& filename) {
    ofstream file(filename);
    if (!file.is_open()) {
        cout << "Erreur: impossible d'ouvrir " << filename << " pour sauvegarde" << endl;
        return;
    }

    file << "# SYSTÈME DE TRANSPORT" << endl;
    file << "# Format: Station > Destination > Louage > Réservations" << endl << endl;

    for (const auto* station : stations) {
        file << "# STATION: " << station->getNom() << endl;
        file << "StationID: " << station->getNumStation() << endl << endl;

        for (const auto* dest : station->getDestinations()) {
            file << "## DESTINATION: " << dest->getNom() << endl;

            // Louages
            for (const Louage& l : dest->getLouages()) {
                file << "Louage|" << l.getSerieVehicule() << "|" << l.getnumero_louage() << "|"
                     << l.getIdProp() << "|" << l.getDestination() << "|" << l.getDepart() << endl;
            }

            // File d'attente
            file << "# FILE_ATTENTE" << endl;

            const ReservationSystem* rs = dest->getReservationSystem();
            if (rs && rs->getQueue()) {
                queue<Reservation*> tempQueue = rs->getQueue()->reservationsQueue;
                while (!tempQueue.empty()) {
                    Reservation* res = tempQueue.front();
                    tempQueue.pop();
                    file << "Reservation|" << res->getNumber() << "|" << res->getUserId() << "|" 
                         << res->getPassengerName() << "|" << res->getDestination() << "|" 
                         << res->getStation() << "|" << (res->isPaid() ? "true" : "false") << "|" 
                         << res->gettimescalled() << endl;
                }
            }

            file << "# FIN_FILE_ATTENTE" << endl << endl;
        }
    }

    file.close();
    cout << "\nSauvegarde terminée dans " << filename << endl;
}

void System::loadFromFile(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cout << "Erreur ouverture: " << filename << endl;
        return;
    }

    cout << "\nChargement depuis: " << filename << endl;

    string line;
    Station* currentStation = nullptr;
    Destination* currentDestination = nullptr;
    string reservationData;
    bool inReservationSection = false;

    while (getline(file, line)) {
        size_t start = line.find_first_not_of(" \t\r\n");
        if (start == string::npos) continue;
        size_t end = line.find_last_not_of(" \t\r\n");
        string trimmed = line.substr(start, end - start + 1);

        // Skip comment lines (but process special markers)
        if (trimmed.empty() || (trimmed[0] == '#' && trimmed.find("# STATION:") != 0 
            && trimmed.find("## DESTINATION:") != 0 
            && trimmed != "# FILE_ATTENTE" 
            && trimmed != "# FIN_FILE_ATTENTE")) {
            continue;
        }

        // NEW STATION
        if (trimmed.find("# STATION: ") == 0) {
            // Save previous destination's reservations if any
            if (currentDestination && inReservationSection && !reservationData.empty()) {
                cout << "    [Sauvegarde avant nouvelle station]" << endl;
                currentDestination->getReservationSystem()->getQueue()->loadFromString(reservationData);
                reservationData.clear();
            }
            inReservationSection = false;

            string stationName = trimmed.substr(11);
            cout << "DEBUG: Tentative de chargement station: " << stationName << endl;
            
            string idLine;
            if (!getline(file, idLine)) {
                cout << "Erreur: fin de fichier inattendue après station " << stationName << endl;
                break;
            }

            size_t startId = idLine.find_first_not_of(" \t\r\n");
            if (startId == string::npos) {
                cout << "DEBUG: Ligne StationID vide" << endl;
                continue;
            }
            string trimmedId = idLine.substr(startId);

            if (trimmedId.find("StationID: ") != 0) {
                cout << "DEBUG: Format StationID invalide: [" << trimmedId << "]" << endl;
                continue;
            }

            int id = stoi(trimmedId.substr(11));
            cout << "DEBUG: ID extrait: " << id << endl;

            Station* newStation = new Station(stationName, id);
            ajouter_station(newStation);
            currentStation = newStation;
            currentDestination = nullptr;

            cout << "Station chargée: " << stationName << " (ID: " << id << ")" << endl;
            continue;
        }

        // NEW DESTINATION
        if (trimmed.find("## DESTINATION: ") == 0) {
            // Save previous destination's reservations if any
            if (currentDestination && inReservationSection && !reservationData.empty()) {
                currentDestination->getReservationSystem()->getQueue()->loadFromString(reservationData);
                reservationData.clear();
            }
            inReservationSection = false;

            if (!currentStation) continue;

            string destName = trimmed.substr(16);

            // Create new destination
            Destination* newDest = new Destination(destName);
            currentStation->ajouterDestination(newDest);
            currentDestination = newDest;

            cout << "  Destination: " << destName << endl;
            continue;
        }

        // LOUAGE
        if (trimmed.find("Louage|") == 0 && currentDestination) {
            vector<string> parts;
            stringstream ss(trimmed);
            string token;
            while (getline(ss, token, '|')) parts.push_back(token);

            if (parts.size() == 6) {
                try {
                    int sv = stoi(parts[1]);
                    int num = stoi(parts[2]);
                    int prop = stoi(parts[3]);
                    string dest = parts[4];
                    string dep = parts[5];
                    Louage l(sv, num, prop, dest, dep);
                    currentDestination->ajouterLouage(l);
                } catch (...) {}
            }
            continue;
        }

        // START RESERVATION SECTION
        if (trimmed == "# FILE_ATTENTE") {
            inReservationSection = true;
            reservationData.clear();
            cout << "    Début file d'attente" << endl;
            continue;
        }

        // END RESERVATION SECTION
        if (trimmed == "# FIN_FILE_ATTENTE") {
            if (currentDestination && !reservationData.empty()) {
                cout << "    Chargement des réservations..." << endl;
                currentDestination->getReservationSystem()->getQueue()->loadFromString(reservationData);
                cout << "    Fin file d'attente" << endl;
            }
            reservationData.clear();
            inReservationSection = false;
            continue;
        }

        // RESERVATION DATA
        if (inReservationSection && trimmed.find("Reservation|") == 0) {
            if (!reservationData.empty()) {
                reservationData += "\n";
            }
            reservationData += trimmed;
        }
    }

    // Final save if file ended without FIN_FILE_ATTENTE
    if (currentDestination && inReservationSection && !reservationData.empty()) {
        cout << "    [Fin de fichier] Chargement des dernières réservations..." << endl;
        currentDestination->getReservationSystem()->getQueue()->loadFromString(reservationData);
    }

    file.close();
    cout << "\nChargement terminé. " << stations.size() << " station(s) chargée(s)." << endl;
    
    // Debug: Show what was loaded
    cout << "Vérification des stations chargées:" << endl;
    for (const auto* station : stations) {
        cout << "  - " << station->getNom() << ": " << station->getDestinations().size() << " destination(s)" << endl;
        for (const auto* dest : station->getDestinations()) {
            cout << "    -> " << dest->getNom() << endl;
        }
    }
    cout << endl;
}

vector<string> System::getAllStationNames() const {
    vector<string> names;
    for (const auto* station : stations) {
        names.push_back(station->getNom());
    }
    return names;
}

vector<string> System::getDestinationsFromStation(const string& stationName) const {
    vector<string> destinations;
    const Station* station = findStation(stationName);
    if (station) {
        for (const auto* dest : station->getDestinations()) {
            destinations.push_back(dest->getNom());
        }
    }
    return destinations;
}

vector<Louage> System::searchLouages(const string& fromStation, const string& toDestination) {
    vector<Louage> results;
    
    Station* station = findStation(fromStation);
    if (!station) {
        cout << "🔍 Recherche: Station '" << fromStation << "' non trouvée" << endl;
        return results;
    }
    
    Destination* dest = findDestination(toDestination, fromStation);
    if (!dest) {
        cout << "🔍 Recherche: Destination '" << toDestination << "' non trouvée depuis '" << fromStation << "'" << endl;
        return results;
    }
    
    const vector<Louage>& louages = dest->getLouages();
    results.insert(results.end(), louages.begin(), louages.end());
    
    cout << "🔍 Recherche: " << results.size() << " louage(s) trouvé(s) pour " 
         << fromStation << " → " << toDestination << endl;
    
    return results;
}
