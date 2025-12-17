#include <iostream>
#include "../include/System.h"
#ifdef _WIN32
#include <windows.h>
#endif

int main() {
#ifdef _WIN32
    // Force console to use UTF-8
    SetConsoleOutputCP(CP_UTF8);
    // Optional: also for input if needed
    SetConsoleCP(CP_UTF8);
#endif

    System sys;

    std::cout << "========================================\n";
    std::cout << "  TEST DE CHARGEMENT system_data.txt   \n";
    std::cout << "========================================\n\n";

    std::cout << "Chargement du fichier 'system_data.txt'...\n";
    sys.loadFromFile("system_data.txt");

    cout << "\n========================================" << endl;
    cout << "        STATISTIQUES DU SYSTÈME        " << endl;
    cout << "========================================" << endl;

    vector<string> stationNames = sys.getAllStationNames();
    
    // Compter le total de destinations et réservations
    int totalDestinations = 0;
    int totalReservations = 0;
    int totalLouages = 0;
    
    for (const string& stationName : stationNames) {
        const Station* station = sys.findStation(stationName);
        if (!station) continue;
        
        vector<string> destinations = sys.getDestinationsFromStation(stationName);
        totalDestinations += destinations.size();
        
        for (const string& destName : destinations) {
            const Destination* dest = sys.findDestination(destName, stationName);
            if (!dest) continue;
            totalReservations += dest->getTailleFileAttente();
            totalLouages += dest->getLouages().size();
        }
    }
    
    cout << "✓ Stations chargées      : " << stationNames.size() << endl;
    cout << "✓ Destinations totales   : " << totalDestinations << endl;
    cout << "✓ Louages disponibles    : " << totalLouages << endl;
    cout << "✓ Réservations en attente: " << totalReservations << endl;
    cout << "========================================\n" << endl;

    for (const string& stationName : stationNames) {
        const Station* station = sys.findStation(stationName);
        if (!station) continue;

        vector<string> destinations = sys.getDestinationsFromStation(stationName);
        
        cout << "=== STATION: " << station->getNom() 
             << " (ID: " << station->getNumStation() << ") ===" << endl;
        cout << "    Nombre de destinations: " << destinations.size() << endl;
        
        int stationReservations = 0;
        int stationLouages = 0;
        
        for (const string& destName : destinations) {
            const Destination* dest = sys.findDestination(destName, stationName);
            if (!dest) continue;
            
            stationReservations += dest->getTailleFileAttente();
            stationLouages += dest->getLouages().size();

            cout << "  ➜ DESTINATION: " << dest->getNom() << endl;
            cout << "     Louages : " << dest->getLouages().size() << " louage(s)" << endl;
            cout << "     Réservations en file : " << dest->getTailleFileAttente() << " réservation(s)" << endl;
        }
        
        cout << "    📊 Total station: " << stationLouages << " louages, " 
             << stationReservations << " réservations" << endl;
        cout << endl;
    }
    
    cout << "========================================" << endl;
    cout << "         TEST TERMINÉ AVEC SUCCÈS      " << endl;
    cout << "========================================" << endl;

    return 0;
}