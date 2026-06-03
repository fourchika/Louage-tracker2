#include "Database.h"
#include <cstdlib>
#include <stdexcept>
#include <iostream>

pqxx::connection* Database::conn = nullptr;

pqxx::connection& Database::get() {
    if (!conn) {
        const char* url = std::getenv("DATABASE_URL");
        if (!url) throw std::runtime_error("DATABASE_URL environment variable not set");
        conn = new pqxx::connection(url);
        std::cout << "✓ Connexion PostgreSQL établie" << std::endl;
    }
    return *conn;
}
