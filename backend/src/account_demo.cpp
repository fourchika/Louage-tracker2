#ifdef _WIN32
#include <windows.h>
#endif

#include <iostream>
#include "../include/AccountManager.h"

using namespace std;

void showMenu() {
    cout << "\n╔════════════════════════════════════╗" << endl;
    cout << "║     LOUAGE TRACKER - LOGIN         ║" << endl;
    cout << "╚════════════════════════════════════╝" << endl;
    cout << "1. Se connecter" << endl;
    cout << "2. Créer un compte" << endl;
    cout << "3. Voir tous les comptes" << endl;
    cout << "4. Sauvegarder les comptes" << endl;
    cout << "5. Quitter" << endl;
    cout << "════════════════════════════════════" << endl;
    cout << "Choix: ";
}

void registerMenu(AccountManager& accountMgr) {
    cout << "\n╔════════════════════════════════════╗" << endl;
    cout << "║       CRÉER UN COMPTE              ║" << endl;
    cout << "╚════════════════════════════════════╝" << endl;
    cout << "1. Passager" << endl;
    cout << "2. Chauffeur" << endl;
    cout << "3. Caissier" << endl;
    cout << "4. Manager" << endl;
    cout << "5. Admin" << endl;
    cout << "0. Retour" << endl;
    cout << "════════════════════════════════════" << endl;
    cout << "Type de compte: ";
    
    int choice;
    cin >> choice;
    cin.ignore();
    
    if (choice == 0) return;
    
    string username, password, email, phone, fullName;
    
    cout << "\nNom d'utilisateur: ";
    getline(cin, username);
    
    cout << "Mot de passe: ";
    getline(cin, password);
    
    cout << "Email: ";
    getline(cin, email);
    
    cout << "Téléphone: ";
    getline(cin, phone);
    
    switch (choice) {
        case 1: { // Passenger
            cout << "Nom complet (optionnel): ";
            getline(cin, fullName);
            accountMgr.registerPassenger(username, password, email, phone, fullName);
            break;
        }
        case 2: { // Driver
            string license;
            int experience;
            cout << "Numéro de permis: ";
            getline(cin, license);
            cout << "Années d'expérience: ";
            cin >> experience;
            cin.ignore();
            accountMgr.registerDriver(username, password, email, phone, license, experience);
            break;
        }
        case 3: { // Cashier
            string station;
            cout << "Nom de la station: ";
            getline(cin, station);
            accountMgr.registerCashier(username, password, email, phone, station);
            break;
        }
        case 4: { // Manager
            accountMgr.registerManager(username, password, email, phone);
            break;
        }
        case 5: { // Admin
            int permLevel;
            cout << "Niveau de permission (1-3): ";
            cin >> permLevel;
            cin.ignore();
            accountMgr.registerAdmin(username, password, email, phone, permLevel);
            break;
        }
        default:
            cout << "Choix invalide!" << endl;
    }
}

void loginMenu(AccountManager& accountMgr) {
    cout << "\n╔════════════════════════════════════╗" << endl;
    cout << "║          CONNEXION                 ║" << endl;
    cout << "╚════════════════════════════════════╝" << endl;
    
    string username, password;
    
    cout << "Nom d'utilisateur: ";
    getline(cin, username);
    
    cout << "Mot de passe: ";
    getline(cin, password);
    
    User* user = accountMgr.login(username, password);
    
    if (user) {
        cout << "\n✓ CONNEXION RÉUSSIE!" << endl;
        user->displayProfile();
        
        // Here you would show the appropriate dashboard based on role
        cout << "\n[Vous seriez redirigé vers le dashboard " << user->getRole() << "]" << endl;
        
        cout << "\nAppuyez sur Entrée pour vous déconnecter...";
        cin.get();
        accountMgr.logout();
    }
}

int main() {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif

    AccountManager accountMgr;
    
    // Load existing accounts from file
    cout << "╔════════════════════════════════════╗" << endl;
    cout << "║   CHARGEMENT DES COMPTES...        ║" << endl;
    cout << "╚════════════════════════════════════╝" << endl;
    accountMgr.loadFromFile("accounts.txt");
    
    int choice;
    bool running = true;
    
    while (running) {
        showMenu();
        cin >> choice;
        cin.ignore(); // Clear newline
        
        switch (choice) {
            case 1:
                loginMenu(accountMgr);
                break;
                
            case 2:
                registerMenu(accountMgr);
                break;
                
            case 3:
                accountMgr.displayAllUsers();
                break;
                
            case 4:
                accountMgr.saveToFile("accounts.txt");
                break;
                
            case 5:
                cout << "\n╔════════════════════════════════════╗" << endl;
                cout << "║   SAUVEGARDE ET FERMETURE...       ║" << endl;
                cout << "╚════════════════════════════════════╝" << endl;
                accountMgr.saveToFile("accounts.txt");
                cout << "Au revoir! 👋" << endl;
                running = false;
                break;
                
            default:
                cout << "✗ Choix invalide!" << endl;
        }
    }
    
    return 0;
}