#ifndef USER_H
#define USER_H

#include <vector>
#include <string>
using namespace std;

const vector<string> VALID_ROLES = {
    "passenger", 
    "cashier",
    "driver",
    "admin",
    "manager"
};

class User{
    protected:
    int ID;
    string username;
    string fullname;
    string password;
    string email;
    string phone;
    string role;

    bool isValidRole(const string& role) const;
    bool isValidEmail(const string& email) const;
    bool isValidPhone(const string& phone) const;
    bool isValidPassword(const string& password) const;

    public:
    User(int id, std::string uname, std::string pwd, 
         std::string email, std::string phone);
    
    virtual bool login(std::string uname, std::string pwd);
    virtual bool changePassword(std::string oldPwd, std::string newPwd);
    virtual void displayProfile() const = 0;
    
    int getUserID() const;
    std::string getUsername() const;
    std::string getRole() const;
    
    virtual ~User() {}
};

#endif