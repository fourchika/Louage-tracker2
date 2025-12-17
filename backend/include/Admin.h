#ifndef ADMIN_H
#define ADMIN_H

#include "User.h"
#include <vector>
#include <string>

using namespace std;

class Admin : public User {
private:
    vector<User*> managedUsers;
    vector<string> systemLogs;
    int permissionLevel;

public:
    Admin(int id, string username, string password, 
          string email, string phone, int permLevel = 3);
    
    bool login(string username, string password) override;
    bool changePassword(string oldPassword, string newPassword) override;
    void displayProfile() const override;
    
    bool createUser(User* newUser);
    bool deleteUser(int userID);
    bool modifyUserRole(int userID, string newRole);
    bool resetUserPassword(int userID, string newPassword);
    void viewAllUsers() const;
    
    void backupSystem() const;
    void restoreSystem() const;
    void viewSystemLogs() const;
    void addSystemLog(string log);
    
    int getPermissionLevel() const;
    bool hasPermission(string action) const;
};

#endif