#include "../include/User.h"
#include <iostream>
#include <regex>

using namespace std;

static string hashPassword(const string& password) {
    string hashed = password;
    for (size_t i = 0; i < hashed.length(); i++) {
        hashed[i] = hashed[i] ^ 0x5A;
    }
    return hashed;
}

User::User(int id, string uname, string pwd, string em, string ph)
    : ID(id), username(uname), password(pwd), email(em), phone(ph), role("passenger") {
    
    if (!isValidEmail(email)) {
        cerr << "⚠️ Email invalide: " << email << endl;
    }
    if (!isValidPhone(phone)) {
        cerr << "⚠️ Téléphone invalide: " << phone << endl;
    }
    if (!isValidPassword(password)) {
        cerr << "⚠️ Mot de passe faible" << endl;
    }
}

bool User::login(string uname, string pwd) {
    if (username == uname) {
        string inputHash = hashPassword(pwd);
        
        if (password == inputHash) {
            cout << "✓ Connexion réussie: " << username << " (" << role << ")" << endl;
            return true;
        }
    }
    return false;
}

bool User::changePassword(string oldPwd, string newPwd) {
    if (password != oldPwd) {
        cout << "✗ Ancien mot de passe incorrect" << endl;
        return false;
    }
    
    if (!isValidPassword(newPwd)) {
        cout << "✗ Nouveau mot de passe invalide (min 6 caractères)" << endl;
        return false;
    }
    
    password = newPwd;
    cout << "✓ Mot de passe changé avec succès" << endl;
    return true;
}

bool User::isValidRole(const string& r) const {
    for (const auto& validRole : VALID_ROLES) {
        if (r == validRole) return true;
    }
    return false;
}

bool User::isValidEmail(const string& em) const {
    regex emailPattern(R"([a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,})");
    return regex_match(em, emailPattern);
}

bool User::isValidPhone(const string& ph) const {
    regex phonePattern(R"([245679]\d{7})");
    return regex_match(ph, phonePattern);
}

bool User::isValidPassword(const string& pwd) const {
    return pwd.length() >= 6;
}

int User::getUserID() const {
    return ID;
}

string User::getUsername() const {
    return username;
}

string User::getRole() const {
    return role;
}