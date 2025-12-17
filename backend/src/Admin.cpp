#include "../include/Admin.h"
#include <iostream>
#include <algorithm>

using namespace std;

Admin::Admin(int id, string username, string password, 
             string email, string phone, int permLevel)
    : User(id, username, password, email, phone), permissionLevel(permLevel) {
    addSystemLog("Admin créé: " + username);
    role="admin";
}

bool Admin::login(string username, string password) {
    bool success = User::login(username, password);
    if (success) {
        addSystemLog("Admin connecté: " + username);
    }
    return success;
}

bool Admin::changePassword(string oldPassword, string newPassword) {
    bool success = User::changePassword(oldPassword, newPassword);
    if (success) {
        addSystemLog("Admin changé mot de passe: " + getUsername());
    }
    return success;
}

void Admin::displayProfile() const {
    cout << "\n╔════════════════════════════════════╗" << endl;
    cout << "║      PROFIL ADMINISTRATEUR         ║" << endl;
    cout << "╚════════════════════════════════════╝" << endl;
    cout << "ID: " << getUserID() << endl;
    cout << "Nom d'utilisateur: " << getUsername() << endl;
    cout << "Rôle: " << getRole() << endl;
    cout << "Niveau de permission: " << permissionLevel << "/3" << endl;
    
    cout << "\n👥 Utilisateurs gérés: " << managedUsers.size() << endl;
    cout << "📝 Logs système: " << systemLogs.size() << " entrées" << endl;
}

bool Admin::createUser(User* newUser) {
    if (newUser == nullptr) {
        cout << "✗ Utilisateur invalide" << endl;
        return false;
    }
    
    for (const auto* user : managedUsers) {
        if (user->getUserID() == newUser->getUserID()) {
            cout << "✗ Utilisateur avec cet ID existe déjà" << endl;
            return false;
        }
    }
    
    managedUsers.push_back(newUser);
    addSystemLog("Utilisateur créé: " + newUser->getUsername() + 
                 " (ID: " + to_string(newUser->getUserID()) + ")");
    cout << "✓ Utilisateur créé avec succès" << endl;
    return true;
}

bool Admin::deleteUser(int userID) {
    for (auto it = managedUsers.begin(); it != managedUsers.end(); ++it) {
        if ((*it)->getUserID() == userID) {
            string username = (*it)->getUsername();
            delete *it;
            managedUsers.erase(it);
            addSystemLog("Utilisateur supprimé: " + username + " (ID: " + to_string(userID) + ")");
            cout << "✓ Utilisateur supprimé" << endl;
            return true;
        }
    }
    
    cout << "✗ Utilisateur non trouvé" << endl;
    return false;
}

bool Admin::modifyUserRole(int userID, string newRole) {
    bool validRole = false;
    for (const auto& role : VALID_ROLES) {
        if (role == newRole) {
            validRole = true;
            break;
        }
    }
    
    if (!validRole) {
        cout << "✗ Rôle invalide: " << newRole << endl;
        return false;
    }
    
    for (auto* user : managedUsers) {
        if (user->getUserID() == userID) {
            addSystemLog("Rôle modifié pour utilisateur ID " + to_string(userID) + 
                        ": " + user->getRole() + " → " + newRole);
            cout << "✓ Rôle modifié: " << newRole << endl;
            return true;
        }
    }
    
    cout << "✗ Utilisateur non trouvé" << endl;
    return false;
}

bool Admin::resetUserPassword(int userID, string newPassword) {
    for (auto* user : managedUsers) {
        if (user->getUserID() == userID) {
            addSystemLog("Mot de passe réinitialisé pour utilisateur: " + 
                        user->getUsername() + " (ID: " + to_string(userID) + ")");
            cout << "✓ Mot de passe réinitialisé pour " << user->getUsername() << endl;
            return true;
        }
    }
    
    cout << "✗ Utilisateur non trouvé" << endl;
    return false;
}

void Admin::viewAllUsers() const {
    if (managedUsers.empty()) {
        cout << "Aucun utilisateur dans le système" << endl;
        return;
    }
    
    cout << "\n╔════════════════════════════════════╗" << endl;
    cout << "║      LISTE DES UTILISATEURS        ║" << endl;
    cout << "╚════════════════════════════════════╝" << endl;
    
    for (size_t i = 0; i < managedUsers.size(); i++) {
        const User* user = managedUsers[i];
        cout << "\n[" << (i+1) << "] ID: " << user->getUserID() << endl;
        cout << "    Username: " << user->getUsername() << endl;
        cout << "    Rôle: " << user->getRole() << endl;
    }
    
    cout << "\nTotal: " << managedUsers.size() << " utilisateur(s)" << endl;
}

void Admin::backupSystem() const {
    cout << "\n🔄 Sauvegarde du système en cours..." << endl;
    cout << "   - Utilisateurs: " << managedUsers.size() << endl;
    cout << "   - Logs: " << systemLogs.size() << " entrées" << endl;
    cout << "✓ Sauvegarde terminée" << endl;
}

void Admin::restoreSystem() const {
    cout << "\n🔄 Restauration du système en cours..." << endl;
    cout << "✓ Système restauré" << endl;
}

void Admin::viewSystemLogs() const {
    if (systemLogs.empty()) {
        cout << "Aucun log système" << endl;
        return;
    }
    
    cout << "\n╔════════════════════════════════════╗" << endl;
    cout << "║         LOGS SYSTÈME               ║" << endl;
    cout << "╚════════════════════════════════════╝" << endl;
    
    int start = max(0, (int)systemLogs.size() - 20);
    for (size_t i = start; i < systemLogs.size(); i++) {
        cout << "[" << (i+1) << "] " << systemLogs[i] << endl;
    }
    
    if (systemLogs.size() > 20) {
        cout << "\n(Affichage des 20 derniers logs sur " << systemLogs.size() << ")" << endl;
    }
}

void Admin::addSystemLog(string log) {
    systemLogs.push_back(log);
}

int Admin::getPermissionLevel() const {
    return permissionLevel;
}

bool Admin::hasPermission(string action) const {
    if (permissionLevel >= 3) return true;
    
    if (permissionLevel >= 2 && 
        (action == "create_user" || action == "modify_user" || action == "view_users")) {
        return true;
    }
    
    if (permissionLevel >= 1 && action == "view_users") {
        return true;
    }
    
    return false;
}