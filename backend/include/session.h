#ifndef SESSION_H
#define SESSION_H

#include <string>
#include <ctime>

struct Session {
    std::string username;
    std::string role;
    time_t createdAt;
    time_t lastActivity;
};

#endif