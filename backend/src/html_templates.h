#ifndef HTML_TEMPLATES_H
#define HTML_TEMPLATES_H

#include <string>
#include "../include/System.h"
#include "../include/session.h"

class AccountManager; 
// Function declarations
std::string getLoginPage(const std::string& error = "");
std::string getSignupPage(const std::string& error = "");
std::string getHomePage(System& system, Session* session);
std::string getDashboardPage(Session* session);
std::string getCommonCSS();
std::string getNavbar(Session* session);
std::string getReservationPage(Session* session, System& system);
std::string getMyReservationsPage(Session* session, System& system);
std::string getPassengerProfilePage(Session* session, AccountManager& accountManager);
#endif