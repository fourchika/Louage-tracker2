#ifndef ACCOUNTMANAGER_H
#define ACCOUNTMANAGER_H

#include <vector>
#include <string>
#include "User.h"
#include "Passenger.h"
#include "Driver.h"
#include "Cashier.h"
#include "Manager.h"
#include "Admin.h"

using namespace std;

class AccountManager {
private:
    vector<Passenger*> passengers;
    vector<Driver*> drivers;
    vector<Cashier*> cashiers;
    vector<Manager*> managers;
    vector<Admin*> admins;
    
    User* currentLoggedInUser;
    int nextUserID;
    
    string hashPassword(const string& password) const;
    bool verifyPassword(const string& password, const string& hash) const;
    string trim(const string& str) const;

public:
    AccountManager();
    ~AccountManager();
    
    // Account Creation
    bool registerPassenger(string username, string password, string email, string phone, string fullName = "");
    bool registerDriver(string username, string password, string email, string phone, string license, int experience);
    bool registerCashier(string username, string password, string email, string phone, string station);
    bool registerManager(string username, string password, string email, string phone);
    bool registerAdmin(string username, string password, string email, string phone, int permLevel = 3);
    
    // Login/Logout
    User* login(string username, string password);
    void logout();
    User* getCurrentUser() const;
    bool isLoggedIn() const;
    
    // Account Management
    bool usernameExists(const string& username) const;
    bool emailExists(const string& email) const;
    User* findUserByUsername(const string& username) const;
    bool deleteAccount(const string& username);
    bool changeUserPassword(const string& username, const string& oldPassword, const string& newPassword);
    
    // File Operations
    bool saveToFile(const string& filename) const;
    bool loadFromFile(const string& filename);
    
    // Statistics
    int getTotalUsers() const;
    int getUsersByRole(const string& role) const;
    void displayAllUsers() const;
    
    // Get vectors (for specific operations)
    vector<Passenger*>& getPassengers();
    vector<Driver*>& getDrivers();
    vector<Cashier*>& getCashiers();
    vector<Manager*>& getManagers();
    vector<Admin*>& getAdmins();
};

#endif